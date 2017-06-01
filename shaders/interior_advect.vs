#version 400
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv;

out vec2 fUV;

void main() {
	fUV = uv;
	gl_Position = vec4(pos, 1.0);
}