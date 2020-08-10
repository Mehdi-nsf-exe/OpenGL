#version 330 core

in vec3 ourColor;

uniform bool useUColor;
uniform float colorR;
uniform float colorG;
uniform float colorB;

out vec4 fragColor;

void main() {
	if (useUColor) {
		fragColor = vec4(colorR, colorG, colorB, 1.0);
	} else {
		fragColor = vec4(ourColor, 1.0);
	}
}