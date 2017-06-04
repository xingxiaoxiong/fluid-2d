#version 330

in vec2 fUV;

out vec4 color;

uniform sampler2D x; 
uniform sampler2D b; 
uniform float alpha;
uniform float rBeta; 
uniform int textureWidth;

void main() {
	ivec2 uv = ivec2(fUV);
	if (fUV.x > 0 && fUV.x < textureWidth - 1 && fUV.y > 0 && fUV.y < textureWidth - 1) {
		vec4 xL = texelFetch(x, uv - ivec2(1, 0), 0);
		vec4 xR = texelFetch(x, uv + ivec2(1, 0), 0);
		vec4 xB = texelFetch(x, uv - ivec2(0, 1), 0);
		vec4 xT = texelFetch(x, uv + ivec2(0, 1), 0);

		vec4 b = texelFetch(b, uv, 0);

		color = (xL + xR + xB + xT + alpha * b) * rBeta;
	} else {
		color = texelFetch(x, uv, 0);
	}
}
