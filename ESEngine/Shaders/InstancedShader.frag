#version 430 core

#define MAX_NUM_TOTAL_LIGHTS 10

struct Material {
	sampler2D texDiffuse;
	sampler2D texSpecular;
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
  vec2 texCoords;
} fs_in;

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
	if(texture(material.texDiffuse, fs_in.texCoords).a < 0.1)
        discard;

	// Properties
    vec3 normal = normalize(fs_in.normal);
    vec3 viewDir = normalize(lights.viewPos.xyz - fs_in.fragPos);

	vec3 result = vec3(0.0, 0.0, 0.0);

	//Calc directional lights
	for(int i = 0; i < lights.directionalLightsLength; i++) {
        result += calcDirectionalLight(lights.directionalLights[i], normal, viewDir);
	}

    //Calc point lights
	for(int i = 0; i < lights.pointLightsLength; i++) {
        result += calcPointLight(lights.pointLights[i], normal, fs_in.fragPos, viewDir);
	}

	//result = vec3(texture(material.texDiffuse, fs_in.texCoords));

	color = vec4(result, 1.0);
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir)
{
	vec3 matAmbient = material.ambient + vec3(texture(material.texDiffuse, fs_in.texCoords));
	vec3 matSpecular = material.specular + vec3(texture(material.texSpecular, fs_in.texCoords));
	vec3 matDiffuse = material.diffuse + vec3(texture(material.texDiffuse, fs_in.texCoords));

    vec3 lightDir = normalize(-light.direction.xyz);
    
	// Ambient
	vec3 ambient = light.ambient.rgb * matAmbient;
    
	// Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse.rgb * diff * matDiffuse;
    
	// Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular.rgb * spec * matSpecular;
    
	return ambient + diffuse + specular;
}  

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 matAmbient = material.ambient + vec3(texture(material.texDiffuse, fs_in.texCoords));
	vec3 matSpecular = material.specular + vec3(texture(material.texSpecular, fs_in.texCoords));
	vec3 matDiffuse = material.diffuse + vec3(texture(material.texDiffuse, fs_in.texCoords));

	// Ambient
    vec3 ambient = light.ambient.rgb * matAmbient;

    // Diffuse 
    vec3 lightDir = normalize(light.position.xyz - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse.rgb * diff * matDiffuse;

	// Specular
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular.rgb * spec * matSpecular;  

	 // Attenuation
    float distance = length(light.position.xyz - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    
	return ambient + diffuse + specular;    
}