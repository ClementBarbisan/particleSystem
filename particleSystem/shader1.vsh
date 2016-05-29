#version 410 core

layout(location = 0) in vec3 vertexPosition;

out Vertex
{
    vec3 fragPos;
} vertex;

void main(void)
{
    gl_Position = vec4(vertexPosition, 1.0);
    vertex.fragPos = gl_Position.xyz;
}   
