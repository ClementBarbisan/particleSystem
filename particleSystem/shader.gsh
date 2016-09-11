#version 410 core
layout(points) in;
layout(triangle_strip, max_vertices=9) out;

in Vertex
{
    vec3 fragPos;
} vertex[];

out vec3 pos;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

vec4 objTetra[5];

void emit_vert(int vert)
{
	gl_Position = projectionMatrix * viewMatrix * objTetra[vert];
	EmitVertex();
}

void main()
{
	vec4 P = vec4(vertex[0].fragPos, 0.0f);
	vec4 I = vec4(0.002f,0.0f,0.0f,0.0f);
	vec4 J = vec4(0.0f,0.002f,0.0f,0.0f);
	vec4 K = vec4(0.0f,0.0f,0.002f,0.0f);
	pos = (projectionMatrix * viewMatrix * P).xyz;
	
	objTetra[0] = P + J;
	objTetra[1] = P + I - K;
	objTetra[2] = P - I - K;
	objTetra[3] = P + K;
	objTetra[4] = P - J;
	
	emit_vert(4);
	emit_vert(3);
	emit_vert(0);
	emit_vert(2);
	emit_vert(4);
	emit_vert(1);
	emit_vert(0);
	emit_vert(3);
	emit_vert(4);
	
//	for (int vert = 0; vert < 10; vert++)
//		emit_vert(vert);
	EndPrimitive();
	
//	objTetra[0] = P+K-I;
//	objTetra[1] = P-K+I;
//	objTetra[2] = P+K+I;
//	objTetra[3] = P+J;
//	
//	for (int vert = 0; vert < 4; vert++)
//		emit_vert(vert);
//	EndPrimitive();
}