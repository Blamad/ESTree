#version 430 core
out vec4 color;

in VS_OUT {
	vec3 texCoords;
} vs_in;

uniform samplerCube cubemap;

void main()
{    
    color = texture(cubemap, vs_in.texCoords);
}