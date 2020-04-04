#version 430 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.4;

void drawNormal(int index) {

    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = gl_in[index].gl_Position + vec4(gs_in[index].normal, 0.0) * MAGNITUDE;
    EmitVertex();
    EndPrimitive();
}

void generateLine(int startVert, int endVert) {
	gl_Position = gl_in[startVert].gl_Position;
    EmitVertex();
    gl_Position = gl_in[endVert].gl_Position;
    EmitVertex();
    EndPrimitive();
}

void main() {
	generateLine(0,1);
	generateLine(1,2);
	generateLine(2,0);
}