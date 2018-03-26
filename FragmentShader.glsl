#version 430 core


out vec4 outColor;

uniform vec3 ray00;
uniform vec3 ray10;
uniform vec3 ray01;
uniform vec3 ray11;
uniform vec2 Resolution;

struct Sphere {
  vec3 center;
  float radius2;
  float center2;
};


layout(std430, binding = 3) buffer c
{
     Sphere objects[];  
}index;


bool intersectSphere(vec3 dir, Sphere s) {
  float dc = dot(dir, s.center);
  dc = dc*dc;
  return dc >= (s.center2 - s.radius2);
}

vec3 getRay() {
  vec2 scrPos = vec2(gl_FragCoord.x/Resolution.x, gl_FragCoord.y/Resolution.y);
  return normalize(mix(mix(ray00, ray01, scrPos.y), mix(ray10, ray11, scrPos.y), scrPos.x));
}

 
void main() {  
 vec3 ray = getRay();
 for (int i=0; i<index.objects.length(); i++) {
    if (intersectSphere(ray, index.objects[i])) 
      outColor = vec4(1);
    else outColor = vec4(0);
  }
}
