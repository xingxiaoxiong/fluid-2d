#version 330

in vec2 fUV;

uniform sampler2D source;

out vec4 color;

void main() {
    color = texture(source, fUV);
}
