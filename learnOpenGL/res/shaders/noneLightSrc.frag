#version 330 core

#define POINT_LIGHTS_NUM 4

struct DirectionalLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

struct SpotLight {
	
	PointLight pointLightProp;

	vec3 direction;
	float innerCutOff;
	float outerCutOff;
};

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

/**
	Calculates the Directional light effect on a fragment.
	@param light The directional light struct.
	@param normal The normal vector of the fragment.
	@param viewDir the Direction of view (the camera's front).
	@returns the color contribution of the light on the fragment.
 */
vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);

/**
	calulates the effect a pointLight has on a fragment.
	@param light The point light struct.
	@param normal The normal vector of the fragment.
	@param fragPos the position of the frament.
	@param viewDir the Direction of view (the camera's front).
	@returns the color contribution of the point light on the fragment.
 */
vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

/**
	Calulates the effect a Spot light has on the fragment.
	@param light The spot light struct.
	@param normal The normal vector of the fragment.
	@param fragPos the position of the frament.
	@param viewDir the Direction of view (the camera's front).
	@returns the color contribution of the spot light on the fragment.
 */
vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform DirectionalLight DirLight;
uniform SpotLight FlashLight;
uniform PointLight PointLights[POINT_LIGHTS_NUM];
uniform Material material;
uniform vec3 ViewPos;

out vec4 FragColor;

void main() {
	
	// porperties
	vec3 normal = normalize(Normal);
	vec3 viewDir = normalize(ViewPos - FragPos);

	// Directional lighting
	vec3 result = calcDirLight(DirLight, normal, viewDir);
	// Flash light
	result += calcSpotLight(FlashLight, normal, FragPos, viewDir);
	// point lights
	for (int i = 0; i < POINT_LIGHTS_NUM; i++) {
		 result += calcPointLight(PointLights[i], normal, FragPos, viewDir);
	}

	FragColor = vec4(result, 1.0);
}

vec3 calcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) {

	vec3 lightDir = normalize(-light.direction);
	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff *  vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	
	vec3 lightDir = normalize(light.position - fragPos);
	// diffuse
	float diff = max(dot(normal, lightDir), 0.0);
	// specular
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// combine results
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
	// attenuation
	float lightFragDistance = length(light.position - fragPos);
	float attenuation = 1.0 /
		(light.constant + light.linear * lightFragDistance + light.quadratic * (lightFragDistance * lightFragDistance));

	return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {

	vec3 lightImpact = vec3(0.0);
	
	vec3 lightDir = normalize(light.pointLightProp.position - fragPos);

	float theta = dot(lightDir, normalize(-light.direction));
	if (theta > light.outerCutOff) {
		lightImpact = calcPointLight(light.pointLightProp, normal, fragPos, viewDir);
		if (theta < light.innerCutOff) {
			float intensity = (theta - light.outerCutOff) / (light.innerCutOff - light.outerCutOff);
			lightImpact *= intensity;
		}
	}
	return lightImpact;
}

