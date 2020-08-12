#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 modelM;
uniform mat4 viewM;
uniform mat4 projectionM;

out vec2 texCoord;

void main() {
	texCoord = aTexCoord;
	gl_Position = projectionM * viewM * modelM * vec4(aPos, 1.0);
}