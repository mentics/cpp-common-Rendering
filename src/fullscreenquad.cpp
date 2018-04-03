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
#include "Camera.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
  
using namespace MenticsGame;

struct WorldObject {
	glm::vec4 pos;
	glm::vec4 vel;
	glm::vec4 acc;
	float radius;
	float ignore1;
	float ignore2;
	float ignore3;
};

static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	-1.0f,  -1.0f, 0.0f,
};

//void window_size_callback(GLFWwindow* window, int width, int height) {
//	glViewport(0, 0, width, height);
//} 
int viewportWidth;
int viewportHeight;



GLFWwindow* init() {
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

	//glfwSetWindowSizeCallback(window, window_size_callback);
	glViewport(0, 0, width, height);
	viewportWidth = width;
	viewportHeight = height;

	return window;
}

GLuint loadComputeShader()
{
	std::string fText = textFileRead("ComputeShader.glsl");

	const char *Text = fText.c_str();

	std::cout << "\n compute shader ------------\n" << Text << std::endl;
	GLuint compute_handle = glCreateProgram();
	GLuint compute_shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute_shader, 1, &Text, NULL);
	glCompileShader(compute_shader);
	glAttachShader(compute_handle, compute_shader);
	//glDeleteShader(compute_shader);
	glLinkProgram(compute_handle);
	validateShader(compute_shader, "ComputeShader.glsl");
	validateProgram(compute_handle);
	return compute_handle;
}

glm::vec4 toGlm(vect3 v)
{
	return glm::vec4(v.x(), v.y(), v.z(), 0);
}
  

Camera cam(glm::vec3(0,0,-10));
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.ProcessKeyboard(FORWARD, 0.1f);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.ProcessKeyboard(BACKWARD, 0.1f);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.ProcessKeyboard(LEFT, 0.1f);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.ProcessKeyboard(RIGHT, 0.1f);
}

double lastX;
double lastY;
bool firstMouse = true;
static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 

	lastX = xpos;
	lastY = ypos;

	cam.ProcessMouseMovement(xoffset, yoffset);
}

struct Sphere {
	glm::vec4 center;
	float radius2;
	float center2;
	glm::vec2 ignore;
};


int main() {
	
	GLFWwindow* window = init();

	Shader shaders("VertexShader.glsl", "FragmentShader.glsl");
	GLuint compute_handle = loadComputeShader();
	float aspectRatio = viewportWidth / (float)viewportHeight;
	
	//// Compute Shader data ////

	GLint cameraId = glGetUniformLocation(compute_handle, "cameraPos");
	GLint gameTimeId = glGetUniformLocation(compute_handle, "gameTime");
	GLint dtId = glGetUniformLocation(compute_handle, "dt");

	// CPU to ComputeShader "World" buffer
	const int dim = 10;
	const int numWorldObjects = dim*dim*dim;
	WorldObject world[numWorldObjects];
	for (int i = -dim / 2; i < dim/2; i++) {
		for (int j = -dim / 2; j < dim/2; j++) {
			for (int k = -dim / 2; k < dim/2; k++) {
				uint64_t ind = dim*dim*(i+dim/2) + dim*(j+dim/2) + k+dim/2;
				world[ind].pos = toGlm(vect3((float)i, (float)j, (float)k + 2*dim));
				//world[ind].pos = toGlm(vect3(0, 0, 0));
				world[ind].vel = toGlm(vect3(0, 1, 0)); 
				world[ind].acc = toGlm(vect3(0, 0, 0));
				world[ind].radius = 0.1f;
			}
		}
	}
	
	GLuint worldId = 0;
	glGenBuffers(1, &worldId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, worldId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(WorldObject) * numWorldObjects, &world, GL_DYNAMIC_COPY);
	std::cout << "World Object 211: " << world[211].pos.x << std::endl;

	// Counter Buffer
	GLuint counterBuffer;
	glGenBuffers(1, &counterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
	glBufferData(GL_ATOMIC_COUNTER_BUFFER, sizeof(GLuint) * 1, NULL, GL_STATIC_DRAW);
	glBindBufferBase(GL_ATOMIC_COUNTER_BUFFER, 1, counterBuffer);
	glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, 0); // unbind the buffer 

	//// Shared data ////

	// ComputeShader to FragmentShader "Index" buffer
	float indexData[numWorldObjects]; // TODO: we shouldn't have to allocate it CPU side
	GLuint indexId = 0;
	glGenBuffers(1, &indexId);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexId);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * 1000, &indexData, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//// Fragment Shader data ////

	GLint Resolution = glGetUniformLocation(shaders.id(), "Resolution");
	GLint ray00Id = glGetUniformLocation(shaders.id(), "ray00");
	GLint ray01Id = glGetUniformLocation(shaders.id(), "ray01");
	GLint ray10Id = glGetUniformLocation(shaders.id(), "ray10");
	GLint ray11Id = glGetUniformLocation(shaders.id(), "ray11");

	// VAO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	
	const int windowSize = 20;  
	uint64_t frameTimes[windowSize];
	uint8_t index = 0;
	uint64_t startNanos = currentTimeNanos();

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, worldId);

	ImGui::CreateContext();

	ImGui_ImplGlfwGL3_Init(window, true);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	//ImGui_ImplGlfwGL3_Init(debugWindow, true);
	GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	do {
		uint64_t nanos = currentTimeNanos();
		frameTimes[index] = nanos;
		uint8_t prevIndex = index - 1 < 0 ? windowSize - 1 : index - 1;
		float dt = (frameTimes[index] - frameTimes[prevIndex]) / (double)windowSize;
		if (index == 0) {
			printf("Average frame time millis: %.4f\n", dt / 1e6);   
		} 
		index = (index + 1) % windowSize;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  
		ImGui_ImplGlfwGL3_NewFrame();
		
		glUseProgram(compute_handle);
		glUniform3f(cameraId, cam.Position.x, cam.Position.y, cam.Position.z);
		float gameTime = (float)(nanos - startNanos) / 1000000000.0;

		ImGui::Begin("test");
		ImGui::Text("Game Time: %.2f", gameTime);
		ImGui::Text("CameraPos %.2f, %.2f, %.2f", cam.Position.x, cam.Position.y, cam.Position.z);
		ImGui::End();

		glUniform1f(gameTimeId, gameTime);
		glUniform1f(dtId, dt);
		glBindBuffer(GL_ATOMIC_COUNTER_BUFFER, counterBuffer);
		glInvalidateBufferData(GL_ATOMIC_COUNTER_BUFFER);
		glClearBufferData(GL_ATOMIC_COUNTER_BARRIER_BIT, GL_UNSIGNED_INT, GL_UNSIGNED_INT, 0, 0);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, worldId);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, indexId);

		//glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo2);
		//GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
		//int n = 0; 
		//memcpy(p, &n, sizeof(GLfloat)*3);
		//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
		
		glDispatchCompute(numWorldObjects, 1, 1);
		glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);

		
		int data = 0;
		memcpy(p, &data, sizeof(int)); 
		

		shaders.bind();
		
		
		glm::vec3 ray00 = glm::normalize(cam.GetViewMatrix() * glm::vec4(-aspectRatio, -1, 1.75, 0));
		glm::vec3 ray10 = glm::normalize(cam.GetViewMatrix() * glm::vec4(aspectRatio, -1, 1.75, 0));
		glm::vec3 ray01 = glm::normalize(cam.GetViewMatrix() * glm::vec4(-aspectRatio, 1, 1.75, 0));
		glm::vec3 ray11 = glm::normalize(cam.GetViewMatrix() * glm::vec4(aspectRatio, 1, 1.75, 0));
		
		glUniform2f(Resolution, (float)viewportWidth, (float)viewportHeight);
		glUniform3f(ray00Id, ray00.x, ray00.y, ray00.z);
		glUniform3f(ray01Id, ray01.x, ray01.y, ray01.z);
		glUniform3f(ray10Id, ray10.x, ray10.y, ray10.z); 
		glUniform3f(ray11Id, ray11.x, ray11.y, ray11.z); 
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, indexId);

		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, indexId);
		Sphere *ptrToIndexData = (Sphere*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY); 
		int testItem = 1;
		std::cout << '[' << ptrToIndexData[testItem].center.x
			<< ',' << ptrToIndexData[testItem].center.y
			<< ',' << ptrToIndexData[testItem].center.z
			<< ',' << ptrToIndexData[testItem].center2
			<< ',' << ptrToIndexData[testItem].radius2
			<< ']';
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


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

		// Draw the triangle
		glDrawArrays(GL_QUADS, 0, 4); 
		ImGui::Render();
		ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
		
		glfwPollEvents();
		glDisableVertexAttribArray(0);
		glfwSwapBuffers(window);
	} while (!glfwGetKey(window, GLFW_KEY_ESCAPE) && !glfwWindowShouldClose(window));  

	glBindVertexArray(0);
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}


