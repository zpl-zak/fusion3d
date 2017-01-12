#version 330

in vec2 uv0;

out vec3 color;

uniform sampler2D texSampler;

void main() {
  color = texture(texSampler, uv0).rgb;
}
