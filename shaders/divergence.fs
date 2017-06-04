#version 330

in vec2 fUV;

out vec4 color;

uniform sampler2D w; 
uniform float halfrdx; 
uniform int textureWidth;

void main() {
	ivec2 uv = ivec2(fUV);
	if (uv.x > 0 && uv.x < textureWidth - 1 && uv.y > 0 && uv.y < textureWidth - 1) {
		vec4 wL = texelFetch(w, uv - ivec2(1, 0), 0);
		vec4 wR = texelFetch(w, uv + ivec2(1, 0), 0);
		vec4 wB = texelFetch(w, uv - ivec2(0, 1), 0);
		vec4 wT = texelFetch(w, uv + ivec2(0, 1), 0);

		color = vec4((wR.x - wL.x + wT.y - wB.y) * halfrdx);
	} else {
		color = texelFetch(w, uv, 0);
	}
}
