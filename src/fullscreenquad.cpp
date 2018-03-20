// windows 10 x64 Visual Studio 17
#include "stdafx.h"
#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glfw3.lib")
//#pragma comment(lib,"glew32.lib")

//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>
#include <string>
#include <fstream>
#include "glm\glm.hpp"

#include "Shader.h"

void window_size_callback(GLFWwindow* window, int width, int height){
	glViewport(0, 0, width, height); 
}


GLuint compute_handle;

void loadComputeShader()
{
	std::string fText = textFileRead("ComputeShader.glsl"); // Read in the vertex shader       

	const char *Text = fText.c_str();

	//cscontent += '\0';
	std::cout << "\n compute shader ------------\n"<< Text << std::endl;
	compute_handle = glCreateProgram();
	GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute_shader, 1, &Text, NULL); 
	glCompileShader(compute_shader);
	glAttachShader(compute_handle, compute_shader);
	//glDeleteShader(compute_shader);
	glLinkProgram(compute_handle);
	validateShader(compute_shader, "ComputeShader.glsl");
	validateProgram(compute_handle);

}

int main() {

	if (!glfwInit()) {
		fprintf(stderr, "Failed to initialize glfw\n");
		return 0;
	}


	
	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	static const int width = mode->width / 2;
	static const int height = mode->height / 2;


	GLFWwindow* window =  glfwCreateWindow(width, height, u8"Test", nullptr, nullptr);

	if (!window) {
		glfwTerminate();
		fprintf(stderr, "Failed to create glfw windows\n");
		return 0;
	}

	

	glfwMakeContextCurrent(window);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_FALSE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, true);
	if (glewInit()) {
		fprintf(stderr, "Failed to initialize GLFW\n");
		glfwDestroyWindow(window);
		glfwTerminate();
		return 0;
	}
	Shader shaders("VertexShader.glsl", "FragmentShader.glsl");

	loadComputeShader();


	
	GLuint textureId;
	glGenTextures(1, &textureId);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId); 
	
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	
	
	glfwSetWindowSizeCallback(window, window_size_callback);
	glViewport(0, 0, width, height);

	GLuint counterBuffer;

	glGenBuffers(1, &counterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 1, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, counterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	// unbind the buffer 
	//glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);

	
	

	do {
		glfwPollEvents();
	
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//glUniform1ui(glGetUniformLocation(compute_handle, "cr"), atomicsBuffer);
		glUseProgram(compute_handle);
		glDispatchCompute(1, 1, 1);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
		//now read the counter
		GLuint *Counter;
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
		Counter = (GLuint*)glMapBufferRange(GL_ATOMIC_COUNTER_BUFFER, 0, sizeof(GLuint) * 1, GL_MAP_READ_BIT);
		printf("counter: %i \n", Counter[0]);
		glUnmapBuffer(GL_ATOMIC_COUNTER_BUFFER);
		

		//glUniform1i(glGetUniformLocation(compute_handle, "tex"), 0);
		//GLfloat *ptrToData = (GLfloat*)glMapBufferRange(GL_TEXTURE_2D, 0, 1, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
		
	
		
		shaders.bind();
		//glUniform1i(glGetUniformLocation(shaders.id(), "tex"), 0); 
		//GLint Resolution =  glGetUniformLocation(P, "Resolution"); 
		
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 3); 
		
	
		glDisableVertexAttribArray(0);



		glfwSwapBuffers(window);
	} while (!glfwGetKey(window, GLFW_KEY_ESCAPE) && !glfwWindowShouldClose(window));



	glBindVertexArray(0);
	glfwDestroyWindow(window);
	while (1) {}
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


