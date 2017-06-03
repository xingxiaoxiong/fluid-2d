#version 330

in vec2 fUV;

out vec4 color;

uniform sampler2D w; 
uniform sampler2D p; 
uniform float halfrdx; 
uniform float halfTexelWidth;

void main() {
	float texelWidth = 2 * halfTexelWidth;
	if (fUV.x > halfTexelWidth && fUV.x < 1 - halfTexelWidth && fUV.y > halfTexelWidth && fUV.y < 1 - halfTexelWidth) {
		float pL = texture(p, fUV - vec2(texelWidth, 0)).x;
		float pR = texture(p, fUV + vec2(texelWidth, 0)).x;
		float pB = texture(p, fUV - vec2(0, texelWidth)).x;
		float pT = texture(p, fUV + vec2(0, texelWidth)).x;

		vec4 v = texture(w, fUV);
		v.xy = v.xy - halfrdx * vec2(pR - pL, pT - pB);

		color = v;
	} else {
		color = texture(w, fUV);
	}
}
