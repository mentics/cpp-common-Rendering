#version 430 core


out vec4 outColor;

layout (std140, binding = 4) uniform data
{ 
	 float r;
	 float g;
	 float b; 
}dat;

void main(){
  
 outColor = vec4(dat.r, dat.g, dat.b, 1.0);
}
