#version 330

in vec2 fUV;

out vec4 color;

uniform sampler2D w; 
uniform sampler2D p; 
uniform float halfrdx; 
uniform int textureWidth;

void main() {
	ivec2 uv = ivec2(fUV);
	if (uv.x > 0 && uv.x < textureWidth - 1 && uv.y > 0 && uv.y < textureWidth - 1) {
		float pL = texelFetch(p, uv - ivec2(1, 0), 0).x;
		float pR = texelFetch(p, uv + ivec2(1, 0), 0).x;
		float pB = texelFetch(p, uv - ivec2(0, 1), 0).x;
		float pT = texelFetch(p, uv + ivec2(0, 1), 0).x;

		vec4 v = texelFetch(w, uv, 0);
		v.xy = v.xy - halfrdx * vec2(pR - pL, pT - pB);

		color = v;
	} else {
		color = texelFetch(w, uv, 0);
	}
}
