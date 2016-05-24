#version 410 core

uniform vec2 mousePos;

out vec4 color;

void main (void)  
{     
	color = vec4(distance(mousePos, gl_PointCoord) * 1.5, 1.0, distance(mousePos, gl_PointCoord) * 1.5, 1.0);
} 
