
#include "stdafx.h"

// ImGui - standalone example application for GLFW + OpenGL 3, using programmable pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan graphics context creation, etc.)
// (GL3W is a helper library to access OpenGL functions since there is no standard header to access modern OpenGL functions easily. Alternatives are GLEW, Glad, etc.)

#include "MenticsCommon.h"
#include "imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include <stdio.h>
#include <GL/glew.h>    // This example is using gl3w to access OpenGL functions (because it is small). You may use glew/glad/glLoadGen/etc. whatever already works for you.
#include <GLFW/glfw3.h>
#include <string>
#include "World.h"
#include "WorldModel.h"
#include "Agent.h"
#include "glm\glm.hpp"
#include "glm\gtc\matrix_transform.hpp"  

// for now
#include "..\..\simulator\src\Scheduler.cpp"

int clr = 0;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

int main(int, char**)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    GLFWwindow* window = glfwCreateWindow(1280, 720, "ImGui OpenGL3 example", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync
    glewInit();

    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
   
    ImGui_ImplGlfwGL3_Init(window, true);

   
    ImGui::StyleColorsDark();
    
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	glm::mat4 view;

	view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f));

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
	
	MenticsGame::World w(0);
	
	TimePoint gameTime = 0;
	TimePoint lastLoopTime;
	TimePoint newLoopTime;

	
	

	lastLoopTime = MenticsGame::currentTimeNanos();
    while (!glfwWindowShouldClose(window))
    {
		newLoopTime = MenticsGame::currentTimeNanos();
		gameTime += newLoopTime - lastLoopTime;
		lastLoopTime = newLoopTime;
        
        glfwPollEvents();
        ImGui_ImplGlfwGL3_NewFrame();

        // 1. Show a simple window.
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
        {
            static float f = 0.0f;
            static int counter = 0;
			ImGui::Text("Game Time : %d | processing Time : %d", gameTime,  getpSched(&w)->getPT());         // Display some text (you can use a format string too)
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our windows open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (NB: most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
        if (show_demo_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
            ImGui::ShowDemoWindow(&show_demo_window);
        }

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

		getp(&w)->agents.bosses.forEach(gameTime,[=](MenticsGame::Agent<>* a) { 
		                                                                                    
			glBegin(GL_TRIANGLES);
			glColor3f(clr, clr, 0);
			glVertex2d(-0.5, -0.5); 
			glVertex2d(0.5, -0.5);  
			glVertex2d(0, 1);  
			glEnd();
			clr += 1;
			
		});

		getp(&w)->agents.minions.forEach(gameTime, [=](MenticsGame::Agent<>* a) {

			glBegin(GL_TRIANGLES);
			glColor3f(clr, clr, 0);
			glVertex2d(-0.5, -0.5);
			glVertex2d(0.5, -0.5);
			glVertex2d(0, 1);
			glEnd();
			clr += 1;

		});

		getp(&w)->agents.quips.forEach(gameTime, [=](MenticsGame::Agent<>* a) {

			glBegin(GL_TRIANGLES);
			glColor3f(clr, clr, 0);
			glVertex2d(-0.5, -0.5);
			glVertex2d(0.5, -0.5);
			glVertex2d(0, 1);
			glEnd();
			clr += 1;

		});

		getp(&w)->agents.shots.forEach(gameTime, [=](MenticsGame::Agent<>* a) {

			glBegin(GL_TRIANGLES);
			glColor3f(clr, clr, 0);
			glVertex2d(-0.5, -0.5);
			glVertex2d(0.5, -0.5);
			glVertex2d(0, 1);
			glEnd();
			clr += 1;

		});

		glBegin(GL_TRIANGLES);
		glColor3f(clr, clr, 0);
		glVertex2d(-0.5, -0.5);
		glVertex2d(0.5, -0.5);
		glVertex2d(0, 1);
		glEnd();
		clr += 1;

		clr = 0;
		

        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
