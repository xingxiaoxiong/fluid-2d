#version 330

in vec2 fUV;

out vec4 color;

uniform sampler2D x; 
uniform sampler2D b; 
uniform float alpha;
uniform float rBeta; 
uniform float halfTexelWidth;

void main() {
	float texWidth = 2 * halfTexelWidth;
	if (fUV.x > halfTexelWidth && fUV.x < 1 - halfTexelWidth && fUV.y > halfTexelWidth && fUV.y < 1 - halfTexelWidth) {
		vec4 xL = texture(x, fUV - vec2(texWidth, 0));
		vec4 xR = texture(x, fUV + vec2(texWidth, 0));
		vec4 xB = texture(x, fUV - vec2(0, texWidth));
		vec4 xT = texture(x, fUV + vec2(0, texWidth));

		vec4 b = texture(x, fUV);

		color = (xL + xR + xB + xT + alpha * b) * rBeta;
	} else {
		color = texture(x, fUV);
	}
}
