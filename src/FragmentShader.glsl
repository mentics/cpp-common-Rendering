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


bool intersectSphere(vec3 dir, Sphere s, out vec4 pt) {
	float dc = dot(dir, s.center.xyz);
	float d = (dc*dc) - (s.center2 - s.radius2);
	if (d >= 0) {
		d = 2 * sqrt(d);
		float t1 = -dc + d;
		float t2 = -dc - d;
		float t = t1 >= 0 && t1 < t2 ? t1 : t2;
		pt = vec4(dir*t, 1);
		return true;
	} else {
		return false;
	}
}

vec3 getRay() {
	vec2 scrPos = vec2(gl_FragCoord.x/Resolution.x, gl_FragCoord.y/Resolution.y);
	return normalize(mix(mix(ray00, ray01, scrPos.y), mix(ray10, ray11, scrPos.y), scrPos.x));
}


void main() {
	vec4 ray = normalize(getRay());
	//outColor = vec4(1, 0.1, 0.1, 1);
	vec4 pt;
	for (int i=0; i<index.objects.length(); i++) {
		if (intersectSphere(ray, index.objects[i], pt)) {
			vec4 normal = normalize(pt - index.objects[i].center);
			float brightness = -dot(ray, normal);
			//outColor = vec4(0, brightness, 0, 1);
			outColor = vec4(0, 1, 0, 1);
			break;
		}
	}
}
