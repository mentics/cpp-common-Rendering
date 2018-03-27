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
#include "MenticsCommon.h"
#include "Shader.h"
#include "glm\gtx\transform.hpp"
#include "World.h" 
  
using namespace MenticsGame;

void window_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
} 


GLuint compute_handle;

void loadComputeShader()
{
	std::string fText = textFileRead("ComputeShader.glsl");

	const char *Text = fText.c_str();


	std::cout << "\n compute shader ------------\n" << Text << std::endl;
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

struct ssbostruct
{
	GLfloat r;
	GLfloat g; 
	GLfloat b;
};

glm::vec3 toGlm(vect3 v)
{
	return glm::vec3(v.x(), v.y(), v.z());
}
 
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);
glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
	glm::vec3(0.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 1.0f, 0.0f));

int main() {
	glm::vec3 eye = { 0, 0, 0 };
	float gameTime = (double)currentTimeNanos() / 1000000000;
	float dt = 1;

	if (!glfwInit()) { 
		fprintf(stderr, "Failed to initialize glfw\n");
		return 0;
	}



	const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	static const int width = mode->width / 2;
	static const int height = mode->height / 2;


	GLFWwindow* window = glfwCreateWindow(width, height, u8"Test", nullptr, nullptr);

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

	// SSBO
	GLuint ssbo = 0;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	
	struct WorldObject {
		float radius;
		glm::vec3 pos;
		glm::vec3 vel;
		glm::vec3 acc;
	};

	// SSBO 2
	WorldObject world[100];

	for(int i = 0; i < 100; i++)
	{
		world[i].pos = toGlm(randomVector(1.0));
		world[i].acc = toGlm(randomVector(1.0));
		world[i].vel = toGlm(randomVector(1.0));
		world[i].radius = 0.5f;
	}

	GLuint ssbo2 = 0;
	glGenBuffers(1, &ssbo2);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(WorldObject) * 100, &world, GL_DYNAMIC_COPY);


	// VAO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f,  -1.0f, 0.0f,
	};

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);



	glfwSetWindowSizeCallback(window, window_size_callback);
	glViewport(0, 0, width, height);


	// Counter Buffer
	GLuint counterBuffer;

	glGenBuffers(1, &counterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 1, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, counterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0);
	// unbind the buffer 


	const int windowSize = 20; 
	uint64_t frameTimes[windowSize];
	uint8_t index = 0;
	do { 
		frameTimes[index] = currentTimeNanos();
		uint8_t prevIndex = index - 1 < 0 ? windowSize - 1 : index - 1;
		double dt = (frameTimes[index] - frameTimes[prevIndex]) / (double)windowSize;
		if (index == 0) {
			printf("Average frame time millis: %.4f\n", dt / 1e6);   
		} 
		index = (index + 1) % windowSize;
		glfwPollEvents();
		
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		
		glUseProgram(compute_handle);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);

		// update ssbo2 
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo2);
		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
		//GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		//int n = 0; 
		//memcpy(p, &n, sizeof(GLfloat)*3);
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		   
		glDispatchCompute(10000000, 1, 1);  
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
		
		GLuint *Counter;  
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer); 
		glInvalidateBufferData(GL_ATOMIC_COUNTER_BUFFER);
		glClearBufferData(GL_ATOMIC_COUNTER_BARRIER_BIT, GL_UNSIGNED_INT, GL_UNSIGNED_INT, 0, 0);
		
		shaders.bind(); 
		GLint Resolution = glGetUniformLocation(shaders.id(), "Resolution");
		int w, h; 
		glfwGetWindowSize(window, &w, &h);      
		glUniform2f(Resolution,(float)w,(float)h); 
		float aspectRatio = w / h;  
		
		glm::vec3 ray00 = glm::normalize(view * glm::vec4(-aspectRatio, -1, 1.75, 0));
		glm::vec3 ray10 = glm::normalize(view * glm::vec4(aspectRatio, -1, 1.75, 0));
		glm::vec3 ray01 = glm::normalize(view * glm::vec4(-aspectRatio, 1, 1.75, 0)); 
		glm::vec3 ray11 = glm::normalize(view * glm::vec4(aspectRatio, 1, 1.75, 0));
		
		glUniform3f(glGetUniformLocation(shaders.id(), "ray00"), ray00.x, ray00.y, ray00.z);
		glUniform3f(glGetUniformLocation(shaders.id(), "ray01"), ray01.x, ray01.y, ray01.z);
		glUniform3f(glGetUniformLocation(shaders.id(), "ray10"), ray10.x, ray10.y, ray10.z); 
		glUniform3f(glGetUniformLocation(shaders.id(), "ray11"), ray11.x, ray11.y, ray11.z); 
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo);
		
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
		glDrawArrays(GL_QUADS, 0, 4); 
		

		glDisableVertexAttribArray(0);


		
		glfwSwapBuffers(window);
	} while (!glfwGetKey(window, GLFW_KEY_ESCAPE) && !glfwWindowShouldClose(window));  



	glBindVertexArray(0);
	glfwDestroyWindow(window);
	while (1) {}
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


