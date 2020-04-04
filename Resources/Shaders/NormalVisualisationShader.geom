#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.8;

void generateTriangle(int index)
{
	float WIDTH = 0.2;
	vec3 normal = gs_in[index].normal;
	vec4 vertex = gl_in[index].gl_Position;

	vertex.x = vertex.x + WIDTH;
	gl_Position = vertex;
	EmitVertex();
	vertex.x = vertex.x - 2*WIDTH;
	gl_Position = vertex;
	EmitVertex();
    gl_Position = vertex + vec4(normal, 0.0) * MAGNITUDE;
    EmitVertex();

    EndPrimitive();
}

void generateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void main()
{
    generateLine(0);
    generateLine(1);
    generateLine(2);
}