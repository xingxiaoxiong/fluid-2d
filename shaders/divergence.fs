#version 330

in vec2 fUV;

out vec4 color;

uniform sampler2D w; 
uniform float halfrdx; 
uniform float halfTexelWidth;

void main() {
	float texWidth = 2 * halfTexelWidth;
	if (fUV.x > halfTexelWidth && fUV.x < 1 - halfTexelWidth && fUV.y > halfTexelWidth && fUV.y < 1 - halfTexelWidth) {
		vec4 wL = texture(w, fUV - vec2(texWidth, 0));
		vec4 wR = texture(w, fUV + vec2(texWidth, 0));
		vec4 wB = texture(w, fUV - vec2(0, texWidth));
		vec4 wT = texture(w, fUV + vec2(0, texWidth));

		color = vec4((wR.x - wL.x + wT.y - wB.y) * halfrdx);
	} else {
		color = texture(w, fUV);
	}
}
