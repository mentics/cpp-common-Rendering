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

void toSphere(WorldObject obj, float gameTime, float gameTime2, Sphere s) {
	vec3 c = obj.pos.xyz + gameTime * obj.vel.xyz + gameTime2 * obj.acc.xyz - cameraPos;
	s.center = vec4(c, 1);
	s.radius2 = obj.radius*obj.radius;
	s.center2 = dot(c, c);
}

void main(){
	float gameTime2 = gameTime*gameTime;
	uint counter = gl_GlobalInvocationID.x;
	toSphere(world.objects[gl_GlobalInvocationID.x], gameTime, gameTime2, index.objects[counter]);
}