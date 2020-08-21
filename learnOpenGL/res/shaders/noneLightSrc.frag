#version 330 core

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform Light light;
uniform Material material;
uniform vec3 ViewPos;

out vec4 FragColor;

void main() {

	// ambient
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

	// diffuse
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	float diff =  max(dot(normal, lightDir), 0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));

	// specular
	vec3 viewDir = normalize(ViewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

	// attenuation
	float distanceToLight = length(light.position - FragPos);
	float attenuation = 1.0 / 
		(light.constant + light.linear * distanceToLight + light.quadratic * (distanceToLight * distanceToLight));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}