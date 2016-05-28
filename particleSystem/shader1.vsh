#version 410 core

layout(location = 0) in vec3 vertexPosition;
uniform mat4 projectionMatrix;
out Vertex
{
    vec3 fragPos;
} vertex;

void main(void)
{
    gl_Position = projectionMatrix * vec4(vertexPosition, 1.0);
    vertex.fragPos = gl_Position.xyz;
}   
