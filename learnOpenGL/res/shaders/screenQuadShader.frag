#version 430 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ScreenSampler;

void main() {
	FragColor = texture(ScreenSampler, TexCoords);
}