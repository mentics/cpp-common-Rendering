#version 430
layout (local_size_x = 1) in;
layout (binding = 1, offset = 0) uniform atomic_uint counter;

layout(std430, binding = 3) buffer c
{
     float arr[];
}clr;



void main() {
	clr.arr[0] = 1.0;
	atomicCounterIncrement(counter);
}