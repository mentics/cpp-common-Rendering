#version 430 core

struct Sphere {
	vec4 center;
	float radius2;
	float center2;
	vec2 ignore;
};

out vec4 outColor;

uniform vec2 Resolution;
uniform vec3 ray00;
uniform vec3 ray10;
uniform vec3 ray01;
uniform vec3 ray11;

layout(std430, binding = 4) buffer Index {
     Sphere objects[1000];
} index;


bool intersectSphere(vec3 dir, Sphere s) {
	float dc = dot(dir, s.center.xyz);
	return (dc*dc) >= (s.center2 - s.radius2);
}

vec3 getRay() {
	vec2 scrPos = vec2(gl_FragCoord.x/Resolution.x, gl_FragCoord.y/Resolution.y);
	return normalize(mix(mix(ray00, ray01, scrPos.y), mix(ray10, ray11, scrPos.y), scrPos.x));
}


void main() {
	vec3 ray = normalize(getRay());
	//for (int i=0; i<index.objects.length(); i++) {
	for (int i = 0; i < 100; i++) {  
		if (intersectSphere(ray, index.objects[i])) {
			outColor = vec4(1, 0, 0, 1);
			break;
		}
		else outColor = vec4(0,1,0,1);
	}
	//if (index.objects[0].center.x != 0) {
	//	outColor = vec4(0, 1, 0, 1);
	//} else {
	//	outColor = vec4(1, 0, 0, 1);
	//}
	//outColor = vec4(index.objects[int(mod(gl_FragCoord.x, 100))].center.x, 0, 0, 1.0);
}
