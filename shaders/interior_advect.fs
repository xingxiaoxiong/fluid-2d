#version 400

in vec2 fUV;

layout (location = 0) out vec4 color;

uniform float timeStep;
uniform float rdx; // 1 / grid scale
uniform sampler2D u; // velocity field
uniform sampler2D x; // the field to advect
uniform int textureWidth;

vec4 bilinear(vec2 uv) {
	float x1 = floor(uv.x);
	float x2 = ceil(uv.x);
	float y1 = floor(uv.y);
	float y2 = ceil(uv.y);

	float y2_y = y2 - uv.y;
	float y2_y1 = y2 - y1;
	float y_y1 = uv.y - y1;
	float x2_x = x2 - uv.x;
	float x2_x1 = x2 - x1;
	float x_x1 = uv.x - x1;

	vec4 q11 = texelFetch(x, ivec2(x1, y1), 0);
	vec4 q21 = texelFetch(x, ivec2(x2, y1), 0);
	vec4 q12 = texelFetch(x, ivec2(x1, y2), 0);
	vec4 q22 = texelFetch(x, ivec2(x2, y2), 0);

	if (x1 == x2 && y1 == y2) {
		return q11;
	}
	else if (x1 == x2) {
		return y2_y / y2_y1 * q11 + y_y1 / y2_y1 * q12;
	}
	else if (y1 == y2) {
		return x2_x / x2_x1 * q11 + x_x1 / x2_x1 * q21;
	}

	return 1.0 / (x2_x1 * y2_y1) * (q11 * x2_x * y2_y + q21 * x_x1 * y2_y + q12 * x2_x * y_y1 + q22 * x_x1 * y_y1);
}

void main() {
	vec2 uv;

	if (fUV.x > 0 && fUV.x < textureWidth - 1 && fUV.y > 0 && fUV.y < textureWidth - 1) {
		uv = fUV - timeStep * rdx * texelFetch(u, ivec2(fUV), 0).xy;
	} else {
		color = texelFetch(x, ivec2(fUV), 0);
		return;
	}

	color = bilinear(uv);

	//ivec2 uv2 = ivec2(ceil(uv));
	//color = texelFetch(x, uv2, 0);
}
