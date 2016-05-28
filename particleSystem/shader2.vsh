#version 410 core

layout(location = 0) in vec3 vertexPosition;
uniform mat4 projectionMatrix;

out vec3 pos;

void main(void)
{
    gl_Position = projectionMatrix * vec4(vertexPosition, 1.0);
    pos = gl_Position.xyz;
}   
