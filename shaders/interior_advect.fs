#version 330

in vec2 fUV;

out vec4 color;

uniform float timeStep;
uniform float rdx; // 1 / grid scale
uniform sampler2D u; // velocity field
uniform sampler2D x; // the field to advect
uniform float halfTexelWidth;

void main() {
	vec2 uv;
	if (fUV.x > halfTexelWidth && fUV.x < 1 - halfTexelWidth && fUV.y > halfTexelWidth && fUV.y < 1 - halfTexelWidth) {
		uv = fUV - timeStep * rdx * texture(u, fUV).xy;
	}

	// uv = fUV;
	color = texture(x, uv);
	// color = vec4(uv, 0, 1.0);
}
