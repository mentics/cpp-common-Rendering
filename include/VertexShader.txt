#version 430 core


layout(location = 0) in vec3 vertexPosition_modelspace;
uniform vec2 Resolution;

void main () {
	gl_Position.xyz = vertexPosition_modelspace;
    gl_Position.w = 1.0;
}
