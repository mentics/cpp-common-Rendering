#version 430
layout (local_size_x = 1) in;
layout (binding = 1, offset = 0) uniform atomic_uint counter;
 
void main() {
	atomicCounterIncrement(counter);
}