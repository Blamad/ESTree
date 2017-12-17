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
	mat4 lightSpace;
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
uniform sampler2D directionalShadingSamples[MAX_NUM_TOTAL_LIGHTS];

out vec4 color;

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, sampler2D shadowTexture);
float calcDirectionalShadow(DirectionalLight light, vec3 normal, vec3 lightDir, sampler2D shadowMap);

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
        result += calcDirectionalLight(lights.directionalLights[i], normal, viewDir, directionalShadingSamples[i]);
	}

    //Calc point lights
	for(int i = 0; i < lights.pointLightsLength; i++) {
        result += calcPointLight(lights.pointLights[i], normal, fs_in.fragPos, viewDir);
	}

	color = vec4(result, 1.0f);
}

vec3 calcDirectionalLight(DirectionalLight light, vec3 normal, vec3 viewDir, sampler2D shadowTexture)
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

	float shadow = calcDirectionalShadow(light, normal, lightDir, shadowTexture);
    vec3 lighting = ambient + (1.0 - shadow) * (diffuse + specular);

	return lighting;
}  

float calcDirectionalShadow(DirectionalLight light, vec3 normal, vec3 lightDir, sampler2D shadowMap) {

	vec4 fragPosLightSpace = light.lightSpace * vec4(fs_in.fragPos, 1.0);

    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;

	// check if z coordinate is beyond light's pane
	 if(projCoords.z > 1.0) {
       return 0.0;
	}

    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	
	//WITH BIAS
	// calculate shadow bias
	float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
	// check whether current frag pos is in shadow
	float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	
	//WITHOUT BIAS
	//float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
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