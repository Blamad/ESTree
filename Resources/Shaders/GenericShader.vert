#version 430 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoords;
layout(location = 4) in mat4 instancedModel;
//layout(location = 8) in mat4 instancedNormalModel;

out VS_OUT {
  vec3 fragPos;
  vec3 normal;
  vec2 texCoords;
} vs_out;

layout (std140, binding = 0) uniform Matricies {
	uniform mat4 projection;
	uniform mat4 view;
};

uniform mat4 model;
uniform mat3 normalModel;
uniform float time;
const float PI48 = 150.796447372;
float pi2wt = PI48*time;

subroutine void renderMode();
subroutine uniform renderMode renderModeSubroutine;

//http://shadersmods.com/sonic-ethers-unbelievable-shaders-mod/ - wave calculation based gbuffers_terrain.vsh
vec3 calcWave(in vec3 pos, in float fm, in float mm, in float ma, in float f0, in float f1, in float f2, in float f3, in float f4, in float f5) {
    vec3 ret;
    float magnitude,d0,d1,d2,d3;
    magnitude = sin(pi2wt*fm + pos.x*0.5 + pos.z*0.5 + pos.y*0.5) * mm + ma;
    d0 = sin(pi2wt*f0);
    d1 = sin(pi2wt*f1);
    d2 = sin(pi2wt*f2);
    ret.x = sin(pi2wt*f3 + d0 + d1 - pos.x + pos.z + pos.y) * magnitude;
    ret.z = sin(pi2wt*f4 + d1 + d2 + pos.x - pos.z + pos.y) * magnitude;
	ret.y = sin(pi2wt*f5 + d2 + d0 + pos.z + pos.y - pos.y) * magnitude;
    return ret;
}

vec3 calcMove(in vec3 pos, in float f0, in float f1, in float f2, in float f3, in float f4, in float f5, in vec3 amp1, in vec3 amp2) {
    vec3 move1 = calcWave(pos      , 0.0027, 0.0400, 0.0400, 0.0127, 0.0089, 0.0114, 0.0063, 0.0224, 0.0015) * amp1;
	vec3 move2 = calcWave(pos+move1, 0.0348, 0.0400, 0.0400, f0, f1, f2, f3, f4, f5) * amp2;
    return move1+move2;
}

subroutine (renderMode)
void singleMesh() {
	vec4 modelSpacePos = model * vec4(position, 1.0f);
	vec4 viewSpacePos = view * modelSpacePos;
	gl_Position = projection * viewSpacePos;

	vs_out.fragPos = vec3(model * vec4(position, 1.0f));
	vs_out.normal = normalModel * normal;
	vs_out.texCoords = texCoords;
}

subroutine (renderMode)
void instancedMesh() {
	vec4 modelSpacePos = instancedModel * vec4(position, 1.0f);
	
	if((texCoords.x == 0 && texCoords.y == 0) || (texCoords.x == 1 && texCoords.y == 0)) {
		modelSpacePos.xyz += calcMove(modelSpacePos.xyz,
				0.041,
				0.070,
				0.044,
				0.038,
				0.063,
				0.000,
				vec3(3.0,1.6,3.0),
				vec3(0.0,0.0,0.0));
	}

	vec4 viewSpacePos = view * modelSpacePos;
	gl_Position = projection * viewSpacePos;

	vs_out.fragPos = vec3(instancedModel * vec4(position, 1.0f));
	vs_out.normal = mat3(transpose(inverse(instancedModel))) * normal;
	vs_out.texCoords = texCoords;
}

void main() {
	renderModeSubroutine();
}