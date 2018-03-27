#version 430
layout (local_size_x = 1) in;

struct WorldObject {
  vec4 pos;
  vec4 vel;
  vec4 acc;
  float radius;
  float ignore1;
  float ignore2;
  float ignore3;
};

struct Sphere {  
  vec4 center; 
  float radius2;
  float center2;
  vec2 ignore;
};

uniform vec3 cameraPos;
uniform float gameTime;
uniform float dt;

layout(binding = 1, offset = 0) uniform atomic_uint counter;

layout(std430, binding = 3) buffer World {
     WorldObject objects[1000];
} world;

layout(std430, binding = 4) buffer Index {
     Sphere objects[1000];
} index;

Sphere toSphere(WorldObject obj, float gameTime, float gameTime2) {
	vec3 c = obj.pos.xyz + gameTime * obj.vel.xyz + gameTime2 * obj.acc.xyz - cameraPos;
	return Sphere(vec4(c, 1), obj.radius*obj.radius, dot(c, c), vec2(-1,-1));
}

void main(){
	float gameTime2 = gameTime*gameTime;
	Sphere sphere = toSphere(world.objects[gl_GlobalInvocationID.x], gameTime, gameTime2);
	//if (mod(gl_LocalInvocationIndex.x, 1) == 0) {
		//uint counter = atomicCounterIncrement(counter);
		uint counter = gl_GlobalInvocationID.x;
		//index.objects[counter].center = vec4(counter,0,10,1);
		//index.objects[counter].radius2 = 0.01;
		//index.objects[counter].center2 = 0;
		index.objects[counter].center = sphere.center;
		index.objects[counter].radius2 = sphere.radius2;
		index.objects[counter].center2 = sphere.center2;
	//}
}