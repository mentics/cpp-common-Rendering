#pragma once

#include <GL\glew.h>
#include <string>

class Shader {

public:
	Shader(); // Default constructor
	Shader(const char *vsFile, const char *fsFile); // Constructor for creating a shader from two shader filenames
	~Shader(); // Deconstructor for cleaning up

	void init(const char *vsFile, const char *fsFile); // Initialize our shader file if we have to
	void bind(); // Bind our GLSL shader program
	void unbind(); // Unbind our GLSL shader program
	unsigned int id(); // Get the identifier for our program

private:
	unsigned int shader_id; // The shader program identifier
	unsigned int shader_vp; // The vertex shader identifier
	unsigned int shader_fp; // The fragment shader identifier

	bool inited; // Whether or not we have initialized the shader

};

void validateShader(GLuint shader, const char* file);
std::string textFileRead(const char *fileName);
void validateProgram(GLuint program);