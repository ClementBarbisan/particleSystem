#version 410 core

layout(location = 0) in vec3 vertexPosition;
uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

out vec3 pos;

void main(void)
{
    gl_Position =  projectionMatrix * viewMatrix * vec4(vertexPosition, 1.0);
    pos = gl_Position.xyz;
}   
