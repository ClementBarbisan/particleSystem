#version 410 core
layout(points) in;
layout(triangle_strip, max_vertices=8) out;

in Vertex
{
    vec3 fragPos;
} vertex[];

out vec3 pos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

vec4 objTetra[4];

void emit_vert(int vert)
{
	gl_Position = projectionMatrix * viewMatrix * objTetra[vert];
	EmitVertex();
}

void main()
{
	vec4 P = vec4(vertex[0].fragPos, 0.0f);
	vec4 I = vec4(0.005f,0.0f,0.0f,0.0f);
	vec4 J = vec4(0.0f,0.005f,0.0f,0.0f);
	vec4 K = vec4(0.0f,0.0f,0.005f,0.0f);
	pos = (projectionMatrix * viewMatrix * P).xyz;
	
	objTetra[0] = P+J;
	objTetra[1] = P+K+I;
	objTetra[2] = P+K-I;
	objTetra[3] = P-K+I;
	
	for (int vert = 0; vert < 4; vert++)
		emit_vert(vert);
	EndPrimitive();
	
	objTetra[0] = P+K-I;
	objTetra[1] = P-K+I;
	objTetra[2] = P+K+I;
	objTetra[3] = P+J;
	
	for (int vert = 0; vert < 4; vert++)
		emit_vert(vert);
	EndPrimitive();
}