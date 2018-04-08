#include "stdafx.h"
#pragma comment(linker, "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glfw3.lib")
//#pragma comment(lib,"glew32.lib")

//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Renderer.h"

using namespace MenticsGame;


int main() {

	Renderer r;
	r.run();
	
}
