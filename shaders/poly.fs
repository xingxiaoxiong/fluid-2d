#version 330

in vec2 fUV;

out vec4 color;

void main() {
    if (fUV.x == 0)
		color = vec4(1.0, 0.0, 0.0, 1.0);
	else if (fUV.y == 0)
		color = vec4(0.0, 1.0, 0.0, 1.0);
	else if (fUV.x == 511)
	    color = vec4(0.0, 0.0, 1.0, 1.0);
	else if (fUV.y == 511)
	    color = vec4(1.0, 0.0, 1.0, 1.0);
	else
	    color = vec4(1.0, 1.0, 1.0, 1.0);

	if (fUV.y == 10)
		color = vec4(1.0, 1.0, 1.0, 1.0);
}
