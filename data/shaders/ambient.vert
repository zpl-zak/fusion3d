#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

uniform mat4 mvp;
uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

out vec2 uv0;
out vec3 normal0;
out vec3 frag0;

void main() {
  gl_Position = mvp * vec4(vertex, 1);
  frag0 = vec3(m * vec4(vertex, 1));
  uv0 = uv;
  normal0 = mat3(transpose(inverse(m))) * normal;
}
