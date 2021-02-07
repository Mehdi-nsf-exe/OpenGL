#version 430 core

in vec3 TexCoords;

uniform samplerCube SkyboxSampler;

out vec4 FragColor;

void main() {
	FragColor = texture(SkyboxSampler, TexCoords);
}