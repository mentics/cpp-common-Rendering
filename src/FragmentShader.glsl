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
     Sphere objects[];
} index;


bool solveQuadratic(in float a, in float b, in float c,
	out float x0, out float x1) {
	float discr = b * b - 4.0 * a * c;
	if (discr < 0.0) return false;
	else if (discr == 0.0) {
		x0 = x1 = -0.5 * b / a;
	}
	else {
		float q = (b > 0.0) ?
			-0.5 * (b + sqrt(discr)) :
			-0.5 * (b - sqrt(discr));
		x0 = q / a;
		x1 = c / q;
	}

	return true;
}

bool intersectSphere2(vec3 dir, Sphere sphere, out vec3 intersection) {
	vec3 L = -sphere.center.xyz;
	float a = 1;
	float b = 2.0 * dot(dir, L);
	float c = dot(L, L) - sphere.radius2;
	float t0, t1;
	if (!solveQuadratic(a, b, c, t0, t1)) {
		return false;
	}

	float t = t0 >= 0 && t0 < t1 ? t0 : t1;
	if (t < 0) {
		return false;
	} else {
		intersection = t * dir;
		return true;
	}
}

bool intersectSphere(vec3 dir, Sphere s, out vec3 pt) {
	float dc = dot(dir, -s.center.xyz);
	float discr = (dc*dc) - (s.center2 - s.radius2);
	if (discr >= 0) {
		float d = 2.0 * sqrt(discr);
		float bb = -2.0*dc;
		float t1 = bb + d;
		float t2 = bb - d;
		float t = 0.5 * (t1 >= 0 && t1 < t2 ? t1 : t2);
		if (t > 0) {
			pt = dir * t;
			return true;
		} else {
			return false;
		}
	} else {
		return false;
	}
}


vec3 getRay() {
	vec2 scrPos = vec2(gl_FragCoord.x/Resolution.x, gl_FragCoord.y/Resolution.y);
	return normalize(mix(mix(ray00, ray01, scrPos.y), mix(ray10, ray11, scrPos.y), scrPos.x));
}


void main() {
	vec3 ray = normalize(getRay());
	outColor = vec4(0.1, 0.1, 0.2, 1);
	vec3 pt;
	for (int i=0; i<index.objects.length(); i++) {
		if (intersectSphere(ray, index.objects[i], pt)) {
			vec3 normal = normalize(pt - index.objects[i].center.xyz);
			float brightness = -dot(ray, normal);
			if (brightness > 0) {
				outColor = vec4(0, brightness, 0, 1);
				break;
			}
		}
	}
}
