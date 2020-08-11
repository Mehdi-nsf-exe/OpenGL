#version 330 core

in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 fragColor;

void main() {
	fragColor = mix(texture(texture0, texCoord), texture(texture1, texCoord), 0.2);
}