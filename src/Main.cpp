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


int main() {
	ImGui::IsKeyReleased(5);

}
