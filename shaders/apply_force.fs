#version 330

in vec2 fUV;

out vec4 color;

uniform sampler2D u; 
uniform float radius; // in clip space
uniform vec2 point; // in clip space
uniform vec2 F; // magnitude in clip space
uniform float halfTexelWidth;

void main() {
	if (fUV.x > halfTexelWidth && fUV.x < 1 - halfTexelWidth && fUV.y > halfTexelWidth && fUV.y < 1 - halfTexelWidth) {
		vec2 pos = (fUV - vec2(0.5, 0.5)) * 0.5;
		vec2 v_xy = F * exp(distance(pos, point) * distance(pos, point) / radius);
		// color = vec4(v_xy.x, v_xy.y, 0, 1.0);
		color = vec4(abs(v_xy.x), abs(v_xy.y), 0, 1.0);
	} else {
		color = texture(u, fUV);
	}
}
