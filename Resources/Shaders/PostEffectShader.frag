#version 430 core
out vec4 FragColor;
  
in vec2 texCoords;

uniform sampler2D screenTexture;

void main()
{ 
    FragColor = texture(screenTexture, texCoords);
	//FragColor = vec4(vec3(1.0 - texture(screenTexture, texCoords)), 1.0);
}