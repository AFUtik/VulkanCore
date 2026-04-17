#version 450

layout(location = 0) in vec3 v_pos;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec4 color;
layout(location = 3) in mat4 model;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 texCoord;

layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projview;
} ubo;

void main() {
	fragColor = color;
	texCoord  = uv; 
	gl_Position = ubo.projview * model * vec4(v_pos, 1.0f);
}