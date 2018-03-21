#version 430
layout (local_size_x = 1) in;
layout (binding = 1, offset = 0) uniform atomic_uint counter;

layout(std430, binding = 3) buffer c
{
     uint r;
	 uint g;
	 uint b;  
}clr;

layout (std140, binding = 4) uniform data
{ 
     float r;
	 float g;
	 float b; 
}dat;

void main() {
	atomicCounterIncrement(counter);
}