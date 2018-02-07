#include "stdafx.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

// Depedencies:
// GLEW http://glew.sourceforge.net/
// GLFW http://www.glfw.org/download.html
// ImGUI https://github.com/ocornut/imgui (no binaries available)

// HACK: Include ImGUI cpp source files to avoid including then in the project.
// Be sure to comment out the gl3w.h and glfw3.h includes in imgui_impl_glfw_gl3.cpp.
// Don't forget to copy glew32.dll and glfw3.dll in the output directory.
#include "imgui.h"
#include "imgui.cpp"
#include "imgui_draw.cpp"
//#include "imconfig.h" // empty by default, user - editable
// OpenGL 3 ImGUI implementation
#include "examples/opengl3_example/imgui_impl_glfw_gl3.h"
#include "examples/opengl3_example/imgui_impl_glfw_gl3.cpp"

static void error_callback(int error, const char* description) {
	std::cerr << description << std::endl;
	system("pause");
}

void resize_callback(GLFWwindow *window, int w, int h) {
	glViewport(0, 0, w, h);
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
	glfwSetFramebufferSizeCallback(window, resize_callback);

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << glewGetErrorString(err) << std::endl;
	}

	glfwSwapInterval(1); // Enable vsync
	ImGui_ImplGlfwGL3_Init(window, true);

	glClearColor(.4f,.4f,.3f,1.f);

	int width, height;
	
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);
	const GLchar *vertex_shader =
		"#version 330 core\n" \
		"layout (location = 0) in vec3 aPos;\n" \
		"void main() {\n"\
		"   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" \
		"}\n\0";

	const GLchar *fragment_shader =
		"#version 330 core\n" \
		"out vec4 FragColor;\n" \
		"void main() {\n" \
		"   FragColor = vec4(1.0f,0.5f,0.2f,1.0f);\n" \
		"}\n\0";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex_shader, nullptr);
	glCompileShader(vertexShader);

	int success;
	char log[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, log);
		std::cerr << "ERROR: vertex compilation failed" << log << std::endl;
	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment_shader, nullptr);
	glCompileShader(fragmentShader);

	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, nullptr, log);
		std::cerr << "ERROR: fragment compilation failed" << log << std::endl;
	}

	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	float triangle[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	GLuint vao;
	glGenVertexArrays(1, &vao);

	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplGlfwGL3_NewFrame();
		ImGui::Text("Hello Mentics!");
		ImGui::Render();

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		glEnableVertexAttribArray(0);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();
	return 0;

}
