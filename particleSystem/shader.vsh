#version 410 core

layout(location = 0) in vec3 vertexPosition;
uniform mat4 projectionMatrix;
//attribute vec3 particleVelocity;
//attribute vec3 particleColor;
//
//varying float mass;
//varying vec3 velocity;
//varying vec3 color;
//
//varying vec3 v;
//varying vec3 lightvec;
//varying vec3 normal;

void main(void)
{

//	normal         = normalize(gl_NormalMatrix * particleVelocity);
//  	v              = vec3(gl_ModelViewMatrix * gl_Vertex);
//  	lightvec       = normalize(gl_LightSource[0].position.xyz - v);
//
//    mass = particleMass;
//    velocity = particleVelocity;
//    color = particleColor;

    gl_Position = projectionMatrix * vec4(vertexPosition, 1.0);
}   
