
#include "stdafx.h"
//
//// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
//// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
//// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
//// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)
//
//#include "MenticsCommon.h"
//#include "imgui.h"
//#include "imgui_impl_glfw_gl3.h"
//#include <stdio.h>
//#include <GL/glew.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
//#include <GLFW/glfw3.h>
//#include <string>
//#include "World.h"
//#include "WorldModel.h"
//#include "Agent.h"
//#include "glm\glm.hpp"
//#include "glm\gtc\matrix_transform.hpp"  
//#include "MenticsMath.h"
//
//// for now
//#include "..\..\simulator\src\Scheduler.cpp"
//
//using namespace MenticsGame;
//
//int clr = 0;
//
//glm::vec3 toGlm(vect3 v)
//{
//	return glm::vec3(v.x(), v.y(), v.z());
//}
//
//static void error_callback(int error, const char* description)
//{
//    fprintf(stderr, "Error %d: %s\n", error, description);
//}
//
//float move = 0.0;
//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
//{
//	if (key == GLFW_KEY_W && action == GLFW_PRESS)
//		move += 0.1;
//		
//}
//
//void drawCube(MenticsGame::vect3);
//
//
//// MAIN ------------------------------------------------------
//int main(int, char**)
//{
//    // Setup window
//    glfwSetErrorCallback(error_callback);
//	
//    if (!glfwInit())
//        return 1;
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
//    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//#if __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
//    glfwMakeContextCurrent(window);
//    glfwSwapInterval(1); // Enable vsync
//    glewInit();
//	glfwSetKeyCallback(window, key_callback);
//    
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//   
//    ImGui_ImplGlfwGL3_Init(window, true);
//
//   
//    ImGui::StyleColorsDark();
//    
//
//    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
//
//    // Main loop
//	
//	MenticsGame::World w(0);
//	
//
//	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
//	glClearDepth(1.0f);                   // Set background depth to farthest
//	glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
//	glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
//	glShadeModel(GL_SMOOTH);   // Enable smooth shading
//	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
//	
//	w.setTimeScale(1.0);
//
//	
//	for (int i = 0; i < 100; i++) {
//		w.createQuip(w.getGameTime() + 1000000, makeTrajRandom(5.0, 0.0, 0.0));
//	}
//
//	while (!glfwWindowShouldClose(window))
//    {
//
//        glfwPollEvents();
//        ImGui_ImplGlfwGL3_NewFrame();
//
//	
//        // 1. Show a simple window.
//        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
//        {
//            static float f = 0.0f;
//            static int counter = 0;
//			ImGui::Text("Game Time : %.2f | processing Time : %d | quips : %d", (double)w.getGameTime()/1000000000,  getpSched(&w)->getPT(), 1000);         // Display some text (you can use a format string too)
//            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
//            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color
//
//        
//            if (ImGui::Button("forwards fast"))    
//                move += 0.5;
//			ImGui::SameLine();
//			if (ImGui::Button("forwards slow"))
//				move += 0.01;
//
//			if (ImGui::Button("backwards fast"))   
//				move -= 0.5;
//			ImGui::SameLine();
//			if (ImGui::Button("backwards slow"))
//				move -= 0.01;
//
//            ImGui::SameLine();
//            ImGui::Text("move = %f", move);
//
//            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//        }
//
//        // Rendering
//        int display_w, display_h;
//        glfwGetFramebufferSize(window, &display_w, &display_h);
//        glViewport(0, 0, display_w, display_h);
//        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		getp(&w)->agents.quips.forEach(w.getGameTime(), [&w](MenticsGame::Agent<>* a) {
//		
//			double secondsGameTime = ((double)w.getGameTime())/1000000000;
//			MenticsGame::vect3 pos;
//			MenticsGame::vect3 vel;
//			a->trajectory.get(secondsGameTime)->posVel(secondsGameTime, pos, vel);
//			drawCube(MenticsGame::vect3(pos.x(), pos.y(), pos.z()));
//		});
//		
//        ImGui::Render();
//        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
//        glfwSwapBuffers(window);
//		std::this_thread::sleep_for(std::chrono::milliseconds(5));
//    }
//
//    // Cleanup
//    ImGui_ImplGlfwGL3_Shutdown();
//    ImGui::DestroyContext();
//    glfwTerminate();
//	while (true) {}
//    return 0;
//}
//
//int a;
//void drawCube(MenticsGame::vect3 pos)
//{
//	a++;
//
//	float x = pos.x();
//	float y = pos.y();
//	float z = pos.z();
//
//	const float sizex = 0.5f;
//	const float sizey = 0.5f;
//	const float sizez = 0.5f;
//
//	glTranslatef(-x, -y, -z);
//	glMatrixMode(GL_PROJECTION_MATRIX);
//	
//	glPushMatrix();
//	glRotatef(a, 1, 1, 0);
//	
//	glBegin(GL_QUADS);
//
//	glColor3f(1.0, 1.0, 0.0);
//
//	// FRONT
//	glVertex3f(-sizex, -sizey, sizez);
//	glVertex3f(sizex, -sizey, sizez);
//	glVertex3f(sizex, sizey, sizez);
//	glVertex3f(-sizex, sizey, sizez);
//
//	// BACK
//	glVertex3f(-sizex, -sizey, -sizez);
//	glVertex3f(-sizex, sizey, -sizez);
//	glVertex3f(sizex, sizey, -sizez);
//	glVertex3f(sizex, -sizey, -sizez);
//
//	glColor3f(0.0, 1.0, 0.0);
//
//	// LEFT
//	glVertex3f(-sizex, -sizey, sizez);
//	glVertex3f(-sizex, sizey, sizez);
//	glVertex3f(-sizex, sizey, -sizez);
//	glVertex3f(-sizex, -sizey, -sizez);
//
//	// RIGHT
//	glVertex3f(sizex, -sizey, -sizez);
//	glVertex3f(sizex, sizey, -sizez);
//	glVertex3f(sizex, sizey, sizez);
//	glVertex3f(sizex, -sizey, sizez);
//
//	glColor3f(0.0, 0.0, 1.0);
//
//	// TOP
//	glVertex3f(-sizex, sizey, sizez);
//	glVertex3f(sizex, sizey, sizez);
//	glVertex3f(sizex, sizey, -sizez);
//	glVertex3f(-sizex, sizey, -sizez);
//
//	// BOTTOM
//	glVertex3f(-sizex, -sizey, sizez);
//	glVertex3f(-sizex, -sizey, -sizez);
//	glVertex3f(sizex, -sizey, -sizez);
//	glVertex3f(sizex, -sizey, sizez);
//	glPushMatrix();
//	
//	
//	glEnd();
//	glPopMatrix();
//	glFlush();
//
//	glTranslatef(x, y, z+move);
//}

	
