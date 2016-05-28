#version 410 core
layout(points) in;
layout(points, max_vertices=1) out;

in Vertex
{
    vec3 fragPos;
} vertex[];

out vec3 pos;

void main()
{
	for (int i = 0; i < gl_in.length(); i++)
	{
		gl_Position = gl_in[i].gl_Position;
		pos = vertex[i].fragPos;
		EmitVertex();
	}
	EndPrimitive();
}