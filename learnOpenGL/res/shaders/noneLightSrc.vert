#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;
uniform mat3 NormalMat;

out vec3 Normal;
out vec3 FragPos;

void main() {
	Normal = NormalMat * aNormal;
	vec4 vertexPos = ModelMat * vec4(aPos, 1.0);
	FragPos = vec3(vertexPos);
	gl_Position = ProjectionMat * ViewMat * vertexPos;
}