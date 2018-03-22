#version 430 core


out vec4 outColor;


layout(std430, binding = 3) buffer c
{
     float arr[];  
}dat;

uniform vec2 Resolution;

void main() {
   vec2 test = vec2(gl_FragCoord.x/Resolution.x, gl_FragCoord.y/Resolution.y);
   outColor = vec4(test.x, test.y, 0, 1.0);
}
