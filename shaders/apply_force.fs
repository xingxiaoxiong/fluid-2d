#version 330

in vec2 fUV;

out vec4 color;

uniform sampler2D u; 
uniform float radius; 
uniform vec2 point; 
uniform vec2 F; 
uniform int textureWidth;

void main() {
	if (fUV.x > 0 && fUV.x < textureWidth - 1 && fUV.y > 0 && fUV.y < textureWidth - 1) {
		vec2 v_xy = F * exp(- distance(fUV, point) * distance(fUV, point) / radius) * 1.0;
		// color = vec4(v_xy.x, v_xy.y, 0, 1.0);
		color = vec4(v_xy.x, v_xy.y, 0, 0) + texelFetch(u, ivec2(fUV), 0);
	} else {
		color = texelFetch(u, ivec2(fUV), 0);
	}
}
