#version 410 core

layout(location = 0) in vec3 vertexPosition;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
out Vertex
{
    vec3 fragPos;
} vertex;

void main(void)
{
    gl_Position = projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
    vertex.fragPos = gl_Position.xyz;
}   
