#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 ModelMat;
uniform mat4 ViewMat;
uniform mat4 ProjectionMat;

out vec2 TexCoords;

void main() {
   TexCoords = aTexCoords;
   gl_Position = ProjectionMat * ViewMat * ModelMat * vec4(aPos, 1.0);
}