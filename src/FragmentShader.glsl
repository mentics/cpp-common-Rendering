#version 430 core


out vec4 outColor;


layout(std430, binding = 3) buffer c
{
     float arr[];  
}dat;

uniform vec2 Resolution;

void main(){
   outColor = vec4(dat.arr[0], dat.arr[1], dat.arr[2], 1.0);
}
