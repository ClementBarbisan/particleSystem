#version 410 core

uniform vec2 mousePos;
in vec3 pos;

out vec4 color;

void main (void)  
{
    float dist = distance(vec3(mousePos, -1.0f), pos) / 1.5f;
    float value = 1.0f - dist * dist * dist;
	color = vec4(value, 1.0f, value, 1.0f);
} 
