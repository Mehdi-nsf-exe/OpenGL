#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (std140, binding = 0) uniform PVMats {
	mat4 ProjectionMat;
	mat4 ViewMat;
};

uniform mat4 ModelMat;
uniform mat3 NormalMat;

out VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} vs_out;

void main() {
	vec4 vertexPos = ModelMat * vec4(aPos, 1.0);
	vs_out.FragPos = vec3(vertexPos);
	vs_out.Normal = NormalMat * aNormal;
	vs_out.TexCoords = aTexCoords;
	gl_Position = ProjectionMat * ViewMat * vertexPos;
}