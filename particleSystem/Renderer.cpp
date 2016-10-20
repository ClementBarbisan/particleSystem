//
//  Renderer.cpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/23/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#include "Renderer.hpp"
#include <vector>
#include "utils.hpp"
#include <cmath>
#include <iostream>

void    Renderer::cubeShape()
{
    size_t size = clObject->getTotalWorkSize();
    clSetKernelArg(clObject->getKernel(0), 1, sizeof(size_t), &size);
    clObject->compute(0);
}

void    Renderer::sphereShape()
{
    size_t size = clObject->getTotalWorkSize();
    clSetKernelArg(clObject->getKernel(1), 1, sizeof(size_t), &size);
    clObject->compute(1);
}

void    Renderer::gravityBehaviour()
{
    gravity = true;
    clObject->compute(3);
}

void	Renderer::changeProgram()
{
    programIndex = (programIndex + 1) % 2;
}

t_pos   &   Renderer::getPosition()
{
    return (*position);
}

void    Renderer::changeShape()
{
    currentShape = (currentShape + 1) % 3;
    funcShape func = mapShapes[currentShape];
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    gravity = false;
	programIndex = 0;
    (this->*func)();
    glFinish();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void    Renderer::createParticles(int nb)
{
    int floatByteSize = 4;
    int positionFloatCount = 4;
    int velocityFloatCount = 4;
    int massFloatCount = 4;
    int floatsPerVertex = positionFloatCount + velocityFloatCount + massFloatCount;
    int vertexFloatSizeInBytes = floatByteSize * floatsPerVertex;
    
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, (nb) * sizeof(Particle), NULL, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexFloatSizeInBytes, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    if ((err = glGetError()) != GL_NO_ERROR)
        throw std::runtime_error(searchError(err) + " : createParticles");
}

void    Renderer::errorShader(GLuint id)
{
    GLint maxLength = 0;
    std::string error = "Error shader : ";
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &maxLength);
    std::vector<GLchar> errorLog(maxLength);
    glGetShaderInfoLog(id, maxLength, &maxLength, &errorLog[0]);
    glDeleteShader(id);
    error.insert(error.end(), errorLog.begin(), errorLog.end());
    throw std::runtime_error(error);
}

void    Renderer::addShader(GLenum typeShader, std::string shader, GLuint *sh)
{
    GLint isCompiled = 0;
    const char *strTmp = shader.c_str();
    *sh = glCreateShader(typeShader);
    glShaderSource(*sh, 1, &strTmp, NULL);
    glCompileShader(*sh);
    glGetShaderiv(*sh, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
        errorShader(*sh);
}

void    Renderer::setDelta(float dt)
{
    delta = dt;
}

void    Renderer::openclComputation(float scale)
{
    cl_float4 tmp = {
        {
            (-0.5f + static_cast<float>(currentMouseX) / static_cast<float>(width)) * scale,
            (0.5f - static_cast<float>(currentMouseY) / static_cast<float>(height)) * scale,
            -1.0f,
            0.0f
        }
    };
    clSetKernelArg(clObject->getKernel(2), 1, sizeof(cl_float4),  &tmp);
    clSetKernelArg(clObject->getKernel(2), 2, sizeof(float),  &mass);
    clSetKernelArg(clObject->getKernel(2), 3, sizeof(float),  &delta);
    clObject->compute(2);
}

float  & Renderer::getMass()
{
	return (mass);
}

void    Renderer::render(int mouseX, int mouseY, bool mouseDown)
{
	float scale = 1.0f;
    if (mouseDown)
        lock = !lock;
    if (!lock)
    {
        currentMouseX = mouseX;
        currentMouseY = mouseY;
    }
    glFinish();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(pId[programIndex]);
    glBindVertexArray(vaoId);
    if (gravity)
	{
		if (programIndex)
		{
            matrix[15] = scale;
			openclComputation(scale * 2);
		}
		else
		{
            matrix[15] = 1.5f;
			openclComputation(scale);
		}
	}
	updateMatrix();
    updateModelMatrix();
    glUniformMatrix4fv(glGetUniformLocation(pId[programIndex], "modelMatrix"), 1, GL_FALSE, modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(pId[programIndex], "viewMatrix"), 1, GL_FALSE, matrixView);
    glUniformMatrix4fv(glGetUniformLocation(pId[programIndex], "projectionMatrix"), 1, GL_FALSE, matrix);
    glUniform2f(glGetUniformLocation(pId[programIndex], "mousePos"), -0.5 + (static_cast<float>(mouseX) / width) , 0.5 - (static_cast<float>(mouseY) / height));
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, nbParticles);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void    Renderer::createProgram(int shader)
{
    pId[shader] = glCreateProgram();
	
    addShader(GL_FRAGMENT_SHADER, fileToString(getCurrentDir() + "/shader.fsh"), &fshId);
	if (shader)
	{
		addShader(GL_VERTEX_SHADER, fileToString(getCurrentDir() + "/shader1.vsh"), &vshId[shader]);
		addShader(GL_GEOMETRY_SHADER, fileToString(getCurrentDir() + "/shader.gsh"), &gshId);
		glAttachShader(pId[shader], gshId);
	}
	else
		addShader(GL_VERTEX_SHADER, fileToString(getCurrentDir() + "/shader2.vsh"), &vshId[shader]);
    glAttachShader(pId[shader], vshId[shader]);
    glAttachShader(pId[shader], fshId);
    glBindAttribLocation(pId[shader], 0, "vertexPosition");
    glLinkProgram(pId[shader]);
    glValidateProgram(pId[shader]);
    if ((err = glGetError()) != GL_NO_ERROR)
        throw std::runtime_error(searchError(err) + " : createProgram.");
}

t_trigo	Renderer::createTrigo()
{
    t_trigo	trigo;
    
    trigo.cos_x = cos(position->rotX);
    trigo.cos_y = cos(position->rotY);
    trigo.cos_z = cos(position->rotZ);
    trigo.sin_x = sin(position->rotX);
    trigo.sin_y = sin(position->rotY);
    trigo.sin_z = sin(position->rotZ);
    trigo.cos_x_sin_y = trigo.cos_x * trigo.sin_y;
    trigo.sin_x_sin_y = trigo.sin_x * trigo.sin_y;
    return (trigo);
}

void    Renderer::updateModelMatrix()
{
    modelMatrix[3] = position->x;
    modelMatrix[7] = position->y;
    modelMatrix[11] = position->z;
}

void	Renderer::updateMatrix()
{
    
    float cosPitch = cos(position->rotX);
    float sinPitch = sin(position->rotX);
    float cosYaw = cos(position->rotY);
    float sinYaw = sin(position->rotY);
    float eye[3] = {position->x, position->y, position->z};
    float xaxis[3] = {cosYaw, 0, -sinYaw};
    float yaxis[3] = {sinYaw * sinPitch, cosPitch, cosYaw * sinPitch};
    float zaxis[3] = {sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw};
    matrixView[0] = xaxis[0];
    matrixView[1] = xaxis[1];
    matrixView[2] = xaxis[2];
    matrixView[3] = -dot(xaxis, eye);
    matrixView[4] = yaxis[0];
    matrixView[5] = yaxis[1];
    matrixView[6] = yaxis[2];
    matrixView[7] = -dot(yaxis, eye);
    matrixView[8] = zaxis[0];
    matrixView[9] = zaxis[1];
    matrixView[10] = zaxis[2];
    matrixView[11] = -dot(zaxis, eye);
    matrixView[12] = 0.0f;
    matrixView[13] = 0.0f;
    matrixView[14] = 0.0f;
    matrixView[15] = 1.0f;
}

GLfloat	*Renderer::identityMatrix()
{
	GLfloat	*mat_rot;
	
	mat_rot = new GLfloat[16];
	mat_rot[0] = 1.0f;
	mat_rot[1] = 0.0f;
	mat_rot[2] = 0.0f;
	mat_rot[3] = 0.0f;
	mat_rot[4] = 0.0f;
	mat_rot[5] = 1.0f;
	mat_rot[6] = 0.0f;
	mat_rot[7] = 0.0f;
	mat_rot[8] = 0.0f;
	mat_rot[9] = 0.0f;
	mat_rot[10] = 1.0f;
	mat_rot[11] = 0.0f;
	mat_rot[12] = 0.0f;
	mat_rot[13] = 0.0f;
	mat_rot[14] = 0.0f;
	mat_rot[15] = 1.0f;
	return (mat_rot);
}


void    Renderer::matrixViewInit()
{
    matrixView = identityMatrix();
}

void    Renderer::initPosition()
{
    position = new t_pos();
    position->x = 0.0f;
    position->y = 0.0f;
    position->z = 0.0f;
    position->rotX = 0.0f;
    position->rotY = 0.0f;
    position->rotZ = 0.0f;
}

void    Renderer::matrixPers()
{
    float	*mat_pers;
    mat_pers = new float[16];
    mat_pers[0] = 1.0f / (((float)width / (float)height) * \
                          tan(90.0f / 2.0f));
    mat_pers[1] = 0.0f;
    mat_pers[2] = 0.0f;
    mat_pers[3] = 0.0f;
    mat_pers[4] = 0.0f;
    mat_pers[5] = 1.0f / tan(90.0f / 2.0f);
    mat_pers[6] = 0.0f;
    mat_pers[7] = 0.0f;
    mat_pers[8] = 0.0f;
    mat_pers[9] = 0.0f;
    mat_pers[10] = (-0.1f - 200.0f) / (0.1f - 200.0f);
    mat_pers[11] = (2.0f * 0.1f * 200.0f) / (0.1f - 200.0f);
    mat_pers[12] = 0.0f;
    mat_pers[13] = 0.0f;
    mat_pers[14] = 1.0f;
    mat_pers[15] = 0.7f;
    matrix = mat_pers;
}

void    Renderer::init(int nb, int currentWidth, int currentHeight)
{
    
    width = currentWidth;
    height = currentHeight;
	programIndex = 0;
    gravity = false;
    lock = false;
    mass = 0.02;
    delta = 0.01;
    initPosition();
    mapShapes = new funcShape[3];
    mapShapes[0] = &Renderer::sphereShape;
    mapShapes[1] = &Renderer::cubeShape;
    mapShapes[2] = &Renderer::gravityBehaviour;
    currentShape = 0;
    matrixViewInit();
    matrixPers();
    nbParticles = nb;
	modelMatrix = identityMatrix();
    createParticles(nb);
	pId = new GLuint[2];
	vshId = new GLuint[2];
    createProgram(0);
	createProgram(1);
    clObject = new CL();
    clObject->shareBuffer(vboId);
    clObject->createProgram("particles.cl");
    clObject->createKernel("cube", nbParticles);
    clObject->createKernel("sphere", nbParticles);
    clObject->createKernel("gravity", nbParticles);
    clObject->createKernel("init_gravity", nbParticles);
    sphereShape();
    
}

Renderer::Renderer(int particlesNb, int width, int height)
{
    init(particlesNb, width, height);
}

Renderer::Renderer()
{
    init(2000000, 800, 600);
}

Renderer::~Renderer()
{
    delete clObject;
    delete mapShapes;
    glDetachShader(pId[0], vshId[0]);
    glDetachShader(pId[0], fshId);
    glDeleteProgram(pId[0]);
	glDetachShader(pId[1], vshId[1]);
	glDetachShader(pId[1], fshId);
	glDetachShader(pId[1], gshId);
	glDeleteProgram(pId[0]);
    glDeleteShader (vshId[0]);
	glDeleteProgram(pId[1]);
	glDeleteShader (vshId[1]);
    glDeleteShader (fshId);
    glDeleteShader (gshId);
    glFinish();
    glDeleteBuffers(1, &vboId);
}
