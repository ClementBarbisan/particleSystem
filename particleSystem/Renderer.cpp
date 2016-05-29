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
//    glVertexAttribPointer(1, velocityFloatCount, GL_FLOAT, GL_FALSE, vertexFloatSizeInBytes, (void *)(sizeof(GL_FLOAT) * 3));
//    glVertexAttribPointer(2, massFloatCount, GL_FLOAT, GL_FALSE, vertexFloatSizeInBytes, (void *)(sizeof(GL_FLOAT) * 6));
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

void    Renderer::openclComputation()
{
    cl_float4 tmp = {
        {
            -0.5f + static_cast<float>(currentMouseX) / static_cast<float>(width),
            0.5f - static_cast<float>(currentMouseY) / static_cast<float>(height),
            -1.0f,
            0.0f
        }
    };
    clSetKernelArg(clObject->getKernel(2), 1, sizeof(cl_float4),  &tmp);
    clSetKernelArg(clObject->getKernel(2), 2, sizeof(float),  &mass);
    clSetKernelArg(clObject->getKernel(2), 3, sizeof(float),  &delta);
    clObject->compute(2);
}

void    Renderer::render(int mouseX, int mouseY, bool mouseDown)
{
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
        openclComputation();
		glUniformMatrix4fv(glGetUniformLocation(pId[programIndex], "viewMatrix"), 1, GL_FALSE, modelMatrix);
	}
	else
	{
		updateMatrix();
		glUniformMatrix4fv(glGetUniformLocation(pId[programIndex], "viewMatrix"), 1, GL_FALSE, matrixView);
	}
    glUniformMatrix4fv(glGetUniformLocation(pId[programIndex], "projectionMatrix"), 1, GL_FALSE, matrix);
    glUniform2f(glGetUniformLocation(pId[programIndex], "mousePos"), static_cast<float>(mouseX) / width , static_cast<float>(mouseY) / height);
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
    glBindAttribLocation(pId[shader], 1, "velocity");
    glBindAttribLocation(pId[shader], 2, "mass");
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

void	Renderer::updateMatrix()
{
    t_trigo	trigo;
    
    trigo = createTrigo();
    matrixView
    [0] = trigo.cos_y * trigo.cos_z;
    matrixView[1] = -trigo.cos_y * trigo.sin_z;
    matrixView[2] = trigo.sin_y;
    matrixView[3] = position->x;
    matrixView[4] = trigo.sin_x_sin_y * trigo.cos_z + trigo.cos_x * \
    trigo.sin_z;
    matrixView[5] = -trigo.sin_x_sin_y * trigo.sin_z + trigo.cos_x * \
    trigo.cos_z;
    matrixView[6] = -trigo.sin_x * trigo.cos_y;
    matrixView[7] = position->y;
    matrixView[8] = -trigo.cos_x_sin_y * trigo.cos_z + trigo.sin_x * \
    trigo.sin_z;
    matrixView[9] = trigo.cos_x_sin_y * trigo.sin_z + trigo.sin_x * \
    trigo.cos_z;
    matrixView[10] = trigo.cos_x * trigo.cos_y;
    matrixView[11] = position->z;
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
                          tan(140.0f / 2.0f));
    mat_pers[1] = 0.0f;
    mat_pers[2] = 0.0f;
    mat_pers[3] = 0.0f;
    mat_pers[4] = 0.0f;
    mat_pers[5] = 1.0f / tan(140.0f / 2.0f);
    mat_pers[6] = 0.0f;
    mat_pers[7] = 0.0f;
    mat_pers[8] = 0.0f;
    mat_pers[9] = 0.0f;
    mat_pers[10] = (0.1f - 100.0f) / (-0.1f - 100.0f);
    mat_pers[11] = (2.0f * -0.1f * 100.0f) / (-0.1f - 100.0f);
    mat_pers[12] = 0.0f;
    mat_pers[13] = 0.0f;
    mat_pers[14] = 1.0f;
    mat_pers[15] = 1.0f;
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
