#version 430 core

layout (location = 0) in vec3 aPos;

layout (std140, binding = 0) uniform PVMats {
	mat4 ProjectionMat;
	mat4 ViewMat;
};

uniform mat4 ModelMat;

void main() {
	gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0);
}