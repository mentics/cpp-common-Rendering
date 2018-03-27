#version 430
layout (local_size_x = 1) in;
layout (binding = 1, offset = 0) uniform atomic_uint counter;

uniform vec3 eye;
uniform float gameTime;
uniform float dt;

struct WorldObject {
  float radius;
  vec3 pos;
  vec3 vel;
  vec3 acc;
};

struct Sphere {  
  vec3 center; 
  float radius2;
  float center2;
};

Sphere toSphere(WorldObject obj, float gameTime, float gameTime2) {
  return Sphere(obj.pos + gameTime * obj.vel + gameTime2 * obj.acc - eye, obj.radius, obj.radius*obj.radius);
}

layout(std430, binding = 4) buffer c
{
     WorldObject objects[];
}world;

layout(std430, binding = 3) buffer d
{
     Sphere objects[];
}index;

void main(){
	float gameTime2 = gameTime*gameTime;
	Sphere sphere = toSphere(world.objects[0], gameTime, gameTime2);
		if (mod(gl_LocalInvocationIndex.x, 1) == 0) {
		  uint counter = atomicCounterIncrement(counter);
		  
		  index.objects[0].center = sphere.center; // <- however this has to work in glsl to copy the sphere data into the index array
		  index.objects[0].radius2 = sphere.radius2;
		}
	
}