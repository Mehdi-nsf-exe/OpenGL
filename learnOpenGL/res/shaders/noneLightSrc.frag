#version 330 core

in vec3 Normal;
in vec3 FragPos;

uniform vec3 ObjectColor;
uniform vec3 LightColor;
uniform vec3 LightPos;
uniform vec3 ViewPos;
uniform float AmbientStrength;
uniform float SpecularStrength;
uniform int Shininess;

out vec4 FragColor;

void main() {
	vec3 ambient = AmbientStrength * LightColor;

	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(LightPos - FragPos);
	float diff =  max(dot(normal, lightDir), 0);
	vec3 diffuse = diff * LightColor;

	vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), Shininess);
	vec3 specular = SpecularStrength * spec * LightColor;

	vec3 resultColor = (ambient + diffuse + specular) * ObjectColor;
	FragColor = vec4(resultColor, 1.0);
}