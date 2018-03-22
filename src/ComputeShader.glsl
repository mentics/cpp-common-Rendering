#version 430
layout (local_size_x = 1) in;
layout (binding = 1, offset = 0) uniform atomic_uint counter;

layout(std430, binding = 4) buffer c
{
     float arr[];
}clrin;

layout(std430, binding = 3) buffer d
{
     float arr[];
}clrout;


void main() {
	if (mod(gl_WorkGroupID.x, 100) == 0) {
		uint tmp = atomicCounterIncrement(counter);
		clrin.arr[tmp] = mod(tmp, 1000) / 1000.0;
	}
}