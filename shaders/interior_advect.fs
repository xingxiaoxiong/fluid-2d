#version 400

in vec2 fUV;

layout (location = 0) out vec3 color;

uniform float timeStep;
uniform float rdx; // 1 / grid scale
uniform sampler2D u; // velocity field
// uniform sampler2D x; // the field to advect
uniform float halfTexelWidth;
uniform sampler2D dye;

void main() {
	vec2 uv;

	if (fUV.x > halfTexelWidth && fUV.x < 1 - halfTexelWidth && fUV.y > halfTexelWidth && fUV.y < 1 - halfTexelWidth) {
		uv = fUV - timeStep * rdx * texture(u, fUV).xy;
	}
	
	color = texture(u, uv).xyz;
}
