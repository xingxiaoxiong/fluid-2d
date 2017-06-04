#version 330

in vec2 fUV;

layout (location = 0) out vec4 color;

uniform int textureWidth;
uniform float scale;  // 1 for pressure, -1 for velocity
uniform sampler2D field;

void main() {
	ivec2 uv;
    if (fUV.x == 0) { // left 
		uv = ivec2(fUV + vec2(1, 0));
	}
	else if (fUV.y == 0) { // bottom
		uv = ivec2(fUV + vec2(0, 1));
	}
	else if (fUV.x == textureWidth - 1) { // right
		uv = ivec2(fUV + vec2(-1, 0));
	}
	else if (fUV.y == textureWidth - 1) { // top
		uv = ivec2(fUV + vec2(0, -1));
	}
	else {
	    uv = ivec2(fUV);
		color = texelFetch(field, uv, 0);
		return;
	}

	//uv = ivec2(fUV);
	color = texelFetch(field, uv, 0) * scale;
}
