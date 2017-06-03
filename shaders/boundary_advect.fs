#version 330

in vec2 fUV;

layout (location = 0) out vec4 color;

uniform float halfTexelWidth;
uniform float scale;  // 1 for pressure, -1 for velocity
uniform sampler2D field;

void main() {
	vec2 uv;
    if (fUV.x <= halfTexelWidth) { // left 
		uv = fUV + vec2(halfTexelWidth * 2, 0);
	}
	else if (fUV.y <= halfTexelWidth) { // bottom
		uv = fUV + vec2(0, halfTexelWidth * 2);
	}
	else if (fUV.x >= 1 - halfTexelWidth) { // right
		uv = fUV + vec2(- halfTexelWidth * 2, 0);
	}
	else if (fUV.y >= 1 - halfTexelWidth) { // up
		uv = fUV + vec2(0, - halfTexelWidth * 2);
	}
	else {
	    uv = fUV;
	}

	//uv = fUV;
	color = texture(field, uv) * scale;
}
