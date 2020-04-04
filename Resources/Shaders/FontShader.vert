#version 430 core
layout (location = 0) in vec4 vertexData;

uniform mat4 projection;

out vec2 TexCoords;

void main()
{
    gl_Position = projection * vec4(vertexData.xy, 0.0, 1.0);
    TexCoords = vertexData.zw;
}