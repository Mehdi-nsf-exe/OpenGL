#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

uniform float xScale;

out vec3 ourColor;

void main() {
	ourColor = aColor;
	gl_Position = vec4(aPos.x * xScale, aPos.y, aPos.z, 1.0);
}