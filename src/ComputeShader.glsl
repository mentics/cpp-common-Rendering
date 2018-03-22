#version 430
layout (local_size_x = 1) in;
layout (binding = 1, offset = 0) uniform atomic_uint counter;

layout(std430, binding = 3) buffer c
{
     float arr[];
}clr;



void main() {
	if (mod(gl_WorkGroupID.x, 100) == 0) {
		uint tmp = atomicCounterIncrement(counter);
		clr.arr[tmp] = mod(tmp, 1000) / 1000.0;
	}
}