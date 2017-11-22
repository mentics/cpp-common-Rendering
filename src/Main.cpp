#include "stdafx.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

// HACK: Include ImGUI cpp source files to avoid including then in the project.
// Be sure to comment out the gl3w.h and glfw3.h includes in imgui_impl_glfw_gl3.cpp.
// Copy glew32.dll and glfw3.dll in the output directory.
#include "imgui.h"
#include "imgui.cpp"
#include "imgui_draw.cpp"
//#include "imconfig.h" // empty by default, user - editable
// OpenGL 3 ImGUI implementation
#include "examples/opengl3_example/imgui_impl_glfw_gl3.h"
#include "examples/opengl3_example/imgui_impl_glfw_gl3.cpp"

static void error_callback(int error, const char* description) {
	std::cerr << description << std::endl;
}

int main() {
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "WordlSim", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	glfwSwapInterval(1); // Enable vsync
	ImGui_ImplGlfwGL3_Init(window, true);

	glClearColor(.4f,.4f,.3f,1.f);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Text("Hello Mentics!");
		ImGui::Render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;

}
