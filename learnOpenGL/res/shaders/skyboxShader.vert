#version 430 core

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

layout (std140, binding = 0) uniform PVMats {
	mat4 ProjectionMat;
	mat4 ViewMat;
};


void main() {
  TexCoords = aPos;
  mat4 skyViewMat = mat4(mat3(ViewMat));
  vec4 pos = ProjectionMat * skyViewMat * vec4(aPos, 1.0);
  gl_Position = pos.xyww;
}