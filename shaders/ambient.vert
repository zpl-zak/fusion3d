#version 330

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;

uniform mat4 mvp;

out vec2 uv0;

void main() {
  gl_Position = mvp * vec4(vertex, 1);
  uv0 = uv;
}
