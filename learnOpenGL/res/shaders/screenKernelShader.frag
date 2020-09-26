#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ScreenSampler;
uniform float Offset;
uniform mat3 Kernel;

vec2 OFFSETS[9] = vec2[](
	vec2(-Offset,  Offset), // top-left
    vec2( 0.0f,    Offset), // top-center
    vec2( Offset,  Offset), // top-right
    vec2(-Offset,  0.0f),   // center-left
    vec2( 0.0f,    0.0f),   // center-center
    vec2( Offset,  0.0f),   // center-right
    vec2(-Offset, -Offset), // bottom-left
    vec2( 0.0f,   -Offset), // bottom-center
    vec2( Offset, -Offset)  // bottom-right
);

void main() {
    vec3 outColor = vec3(0);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            outColor += Kernel[i][j] * vec3(texture(ScreenSampler, TexCoords.st + OFFSETS[i * 3 + j]));
        }
    }
	FragColor = vec4(outColor, 1.0);
}