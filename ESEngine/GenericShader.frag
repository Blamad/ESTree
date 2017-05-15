#version 430 core

#define MAX_NUM_TOTAL_LIGHTS 10

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct PointLight {
	vec4 position;
    vec4 ambient;
    vec4 diffuse;
	vec4 specular;
	float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec4 direction;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

in VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec3 color;
} vs_in;

layout (std140, binding = 1) uniform Lights
{
	PointLight pointLights[MAX_NUM_TOTAL_LIGHTS];
	DirectionalLight directionalLights[MAX_NUM_TOTAL_LIGHTS];
	vec4 viewPos;
	int pointLightsLength;
	int directionalLightsLength;

} lights;

uniform Material material;

out vec4 color;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir);

void main()
{
	// Properties
    vec3 normal = normalize(vs_in.normal);
    vec3 viewDir = normalize(lights.viewPos.xyz - vs_in.fragPos);

	vec3 result;

	//Calc diffuse lights
	for(int i = 0; i < lights.directionalLightsLength; i++)
        result += calcDirectionalLight(lights.directionalLights[i], normal, viewDir);

    //Calc point lights
	for(int i = 0; i < lights.pointLightsLength; i++)
        result += calcPointLight(lights.pointLights[i], normal, vs_in.fragPos, viewDir);

	color = vec4(result, 1.0);
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction.xyz);
    
	// Ambient
	vec3 ambient  = light.ambient.rgb * material.diffuse;
    
	// Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse  = light.diffuse.rgb * diff * material.diffuse;
    
	// Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular.rgb * spec * material.specular;
    
	return (ambient + diffuse + specular);
}  

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	// Ambient
    vec3 ambient = light.ambient.rgb * material.ambient;

    // Diffuse 
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse.rgb * material.diffuse * diff;

	// Specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular.rgb * spec * material.specular;  

	 // Attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
	return (ambient + diffuse + specular);    
}