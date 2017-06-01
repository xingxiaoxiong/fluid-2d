#version 330

in vec2 fUV;

out vec4 color;

float pixelHalfWidth = 1.0 / 1024;

void main() {
    if (fUV.x <= pixelHalfWidth)
		color = vec4(1.0, 0.0, 0.0, 1.0);
	else if (fUV.y <= pixelHalfWidth)
		color = vec4(0.0, 1.0, 0.0, 1.0);
	else if (fUV.x >= 1 - pixelHalfWidth)
	    color = vec4(0.0, 0.0, 1.0, 1.0);
	else if (fUV.y >= 1 - pixelHalfWidth)
	    color = vec4(1.0, 0.0, 1.0, 1.0);
	else
	    color = vec4(1.0, 1.0, 1.0, 1.0);
}
