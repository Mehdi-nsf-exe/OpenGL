#version 430 core

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
	bool hasSpecular;
};

struct FragmentInfo {
	vec4 diffuse;
	vec3 specular;
	vec3 position;
	vec3 normal;
};

/**
	Calculates the Directional light effect on a fragment.
 */
vec3 calcDirLight(DirectionalLight light, FragmentInfo fragInfo, vec3 viewDir);

/**
	calulates the effect a pointLight has on a fragment.
 */
vec3 calcPointLight(PointLight light, FragmentInfo fragInfo, vec3 viewDir);

/**
	Calulates the effect a Spot light has on the fragment.
 */
vec3 calcSpotLight(SpotLight light, FragmentInfo fragInfo, vec3 viewDir);

in VS_OUT {
	vec3 FragPos;
	vec3 Normal;
	vec2 TexCoords;
} fs_in;

uniform DirectionalLight DirLight;
uniform SpotLight FlashLight;
uniform PointLight PointLights[POINT_LIGHTS_NUM];
uniform Material material;
uniform vec3 ViewPos;

out vec4 FragColor;

void main() {
	
	// porperties
	FragmentInfo fragInfo;
	fragInfo.diffuse = texture(material.diffuse, fs_in.TexCoords);
	if (fragInfo.diffuse.a < 0.01) {
		discard;
	}
	fragInfo.specular = vec3(texture(material.specular, fs_in.TexCoords));
	fragInfo.position = fs_in.FragPos;
	fragInfo.normal = normalize(fs_in.Normal);

	vec3 viewDir = normalize(ViewPos - fs_in.FragPos);

	if (dot(viewDir, fragInfo.normal) <= 0) {
		fragInfo.normal = -fragInfo.normal;
	}

	// Directional lighting
	vec3 result = calcDirLight(DirLight, fragInfo, viewDir);
	// Flash light
	result += calcSpotLight(FlashLight, fragInfo, viewDir);
	// point lights
	for (int i = 0; i < POINT_LIGHTS_NUM; i++) {
		result += calcPointLight(PointLights[i], fragInfo, viewDir);
	}

	FragColor = vec4(result, fragInfo.diffuse.a);
}

vec3 calcDirLight(DirectionalLight light, FragmentInfo fragInfo, vec3 viewDir) {

	vec3 lightDir = normalize(-light.direction);
	// diffuse
	float diff = max(dot(fragInfo.normal, lightDir), 0.0);
	// specular
	vec3 reflectDir = reflect(-lightDir, fragInfo.normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//combine results
	vec3 ambient = light.ambient * vec3(fragInfo.diffuse);
	vec3 diffuse = light.diffuse * diff * vec3(fragInfo.diffuse);
	vec3 specular = light.specular * spec * fragInfo.specular;
	return (ambient + diffuse + specular);
}

vec3 calcPointLight(PointLight light, FragmentInfo fragInfo, vec3 viewDir) {
	
	vec3 lightDir = normalize(light.position - fragInfo.position);
	// diffuse
	float diff = max(dot(fragInfo.normal, lightDir), 0.0);
	// specular
	vec3 reflectDir = reflect(-lightDir, fragInfo.normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// combine results
	vec3 ambient = light.ambient * vec3(fragInfo.diffuse);
	vec3 diffuse = light.diffuse * diff * vec3(fragInfo.diffuse);
	vec3 specular = light.specular * spec * fragInfo.specular;
	// attenuation
	float lightFragDistance = length(light.position - fragInfo.position);
	float attenuation = 1.0 /
		(light.constant + light.linear * lightFragDistance + light.quadratic * (lightFragDistance * lightFragDistance));

	return (ambient + diffuse + specular) * attenuation;
}

vec3 calcSpotLight(SpotLight light, FragmentInfo fragInfo, vec3 viewDir) {

	vec3 lightImpact = vec3(0.0);
	
	vec3 lightDir = normalize(light.pointLightProp.position - fragInfo.position);

	float theta = dot(lightDir, normalize(-light.direction));
	if (theta > light.outerCutOff) {
		lightImpact = calcPointLight(light.pointLightProp, fragInfo, viewDir);
		if (theta < light.innerCutOff) {
			float intensity = (theta - light.outerCutOff) / (light.innerCutOff - light.outerCutOff);
			lightImpact *= intensity;
		}
	}
	return lightImpact;
}

