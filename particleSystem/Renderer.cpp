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

void    Renderer::changeShape()
{
    currentShape = (currentShape + 1) % 3 ;
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

void    Renderer::openclComputation()
{
    cl_float4 tmp = {
        {
            0.5f - static_cast<float>(currentMouseX) / static_cast<float>(width),
            -0.5f + static_cast<float>(currentMouseY) / static_cast<float>(height),
            -1.0f,
            0.0f
        }
    };
    clSetKernelArg(clObject->getKernel(2), 1, sizeof(cl_float4),  &tmp);
    clSetKernelArg(clObject->getKernel(2), 2, sizeof(float),  &mass);
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
    glUseProgram(pId);
    glBindVertexArray(vaoId);
    if (gravity)
        openclComputation();
    glUniformMatrix4fv(glGetUniformLocation(pId, "projectionMatrix"), 1, GL_FALSE, matrix);
    glUniform2f(glGetUniformLocation(pId, "mousePos"), static_cast<float>(mouseX) / width , static_cast<float>(mouseY) / height);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, nbParticles);
    glDisableVertexAttribArray(0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void    Renderer::createProgram()
{
    pId = glCreateProgram();
    addShader(GL_VERTEX_SHADER, fileToString(getCurrentDir() + "/shader.vsh"), &vshId);
    addShader(GL_FRAGMENT_SHADER, fileToString(getCurrentDir() + "/shader.fsh"), &fshId);
    addShader(GL_GEOMETRY_SHADER, fileToString(getCurrentDir() + "/shader.gsh"), &gshId);
    glAttachShader(pId, vshId);
    glAttachShader(pId, fshId);
    glAttachShader(pId, gshId);
    glBindAttribLocation(pId, 0, "vertexPosition");
    glBindAttribLocation(pId, 1, "velocity");
    glBindAttribLocation(pId, 2, "mass");
    glLinkProgram(pId);
    glValidateProgram(pId);
    if ((err = glGetError()) != GL_NO_ERROR)
        throw std::runtime_error(searchError(err) + " : createProgram.");
}

void    Renderer::init(int nb, int currentWidth, int currentHeight)
{
    float	*mat_pers;
    width = currentWidth;
    height = currentHeight;
    gravity = false;
    lock = false;
    mass = 0.05;
    mat_pers = new float[16];
    mat_pers[0] = 1.0f / (((float)width / (float)height) * \
                          tan(180.0f / 2.0f));
    mat_pers[1] = 0.0f;
    mat_pers[2] = 0.0f;
    mat_pers[3] = 0.0f;
    mat_pers[4] = 0.0f;
    mat_pers[5] = 1.0f / tan(180.0f / 2.0f);
    mat_pers[6] = 0.0f;
    mat_pers[7] = 0.0f;
    mat_pers[8] = 0.0f;
    mat_pers[9] = 0.0f;
    mat_pers[10] = (0.025f - 150.0f) / (-0.025f - 150.0f);
    mat_pers[11] = (2.0f * -0.025f * 150.0f) / (-0.025f - 150.0f);
    mat_pers[12] = 0.0f;
    mat_pers[13] = 0.0f;
    mat_pers[14] = 1.0f;
    mat_pers[15] = 0.3f;
    mapShapes = new funcShape[3];
    mapShapes[0] = &Renderer::sphereShape;
    mapShapes[1] = &Renderer::cubeShape;
    mapShapes[2] = &Renderer::gravityBehaviour;
    currentShape = 0;
    matrix = mat_pers;
    nbParticles = nb;
    createParticles(nb);
    createProgram();
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
    glDetachShader(pId, vshId);
    glDetachShader(pId, fshId);
    glDeleteProgram(pId);
    glDeleteShader (vshId);
    glDeleteShader (fshId);
    glDeleteShader (gshId);
    glFinish();
    glDeleteBuffers(1, &vboId);
}
