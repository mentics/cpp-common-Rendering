#version 430
layout (local_size_x = 1) in;

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

uniform vec3 eye;
uniform float gameTime;
uniform float dt;

layout(binding = 1, offset = 0) uniform atomic_uint counter;

layout(std430, binding = 3) buffer World {
     WorldObject objects[100];
} world;

layout(std430, binding = 4) buffer Index {
     Sphere objects[100];
} index;

Sphere toSphere(WorldObject obj, float gameTime, float gameTime2) {
	return Sphere(obj.pos + gameTime * obj.vel + gameTime2 * obj.acc - eye, obj.radius, obj.radius*obj.radius);
}

void main(){
	float gameTime2 = gameTime*gameTime;
	Sphere sphere = toSphere(world.objects[gl_LocalInvocationIndex.x], gameTime, gameTime2);
	//if (mod(gl_LocalInvocationIndex.x, 1) == 0) {
		uint counter = atomicCounterIncrement(counter);
		index.objects[counter].center = sphere.center;
		index.objects[counter].radius2 = sphere.radius2;
		index.objects[counter].center2 = dot(sphere.center, sphere.center);
	//}
}