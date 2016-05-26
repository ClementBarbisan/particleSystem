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
    Particle *particles = (Particle *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int i; i < nbParticles; i++)
    {
        float rand1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        particles->x = rand1 >= 0.66 ? (rand() % 2) > 0 ? 0.5 : -0.5 : -0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        particles->y = rand1 < 0.66 && rand1 >= 0.33 ? (rand() % 2) > 0 ? 0.5 : -0.5 : -0.5 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        particles->z = rand1 < 0.33 ? (rand() % 2) > 0 ? 0.0 : -1.0 : -1.0 + static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        particles->velocity_x = 0.0;
        particles->velocity_y = 0.0;
        particles->velocity_z = 0.0;
        particles->mass = 0.0;
        particles->r = 0.0;
        particles->b = 0.0;
        particles++;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void    Renderer::sphereShape()
{
    Particle *particles = (Particle *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int i; i < nbParticles; i++)
    {
        float rand1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 180);
        float rand2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX / 360);
        particles->x = sin(rand1) * cos(rand2);
        particles->y = sin(rand1) * sin(rand2);
        particles->z = cos(rand1);
        particles->velocity_x = 0.0;
        particles->velocity_y = 0.0;
        particles->velocity_z = 0.0;
        particles->mass = 0.0;
        particles->r = 0.0;
        particles->b = 0.0;
        particles++;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void    Renderer::gravityBehaviour()
{
    gravity = true;
    Particle *particles = (Particle *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int i; i < nbParticles; i++)
    {
        particles->velocity_x = (rand()%1000/1000.-.5)*.1;
        particles->velocity_y = (rand()%1000/1000.-.5)*.1;
        particles->velocity_z = (rand()%1000/1000.-.5)*.1;
        particles->mass = 0.01;
        particles++;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
}

void    Renderer::changeShape()
{
    currentShape = (currentShape + 1) % 3 ;
    funcShape func = mapShapes[currentShape];
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    gravity = false;
    (this->*func)();
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
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
    glBufferData(GL_ARRAY_BUFFER, (nb + 1) * sizeof(Particle), NULL, GL_STATIC_DRAW);
    sphereShape();
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
    float tmp = static_cast<float>(currentMouseX) / width;
    clSetKernelArg(clObject->getKernel(), 1, sizeof(float),  &tmp);
    tmp = static_cast<float>(currentMouseY) / height;
    clSetKernelArg(clObject->getKernel(), 2, sizeof(float),  &tmp);
    clObject->compute();
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
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(pId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    if (gravity)
        openclComputation();
    glUniformMatrix4fv(glGetUniformLocation(pId, "projectionMatrix"), 1, GL_FALSE, matrix);
    glUniform2f(glGetUniformLocation(pId, "mousePos"), static_cast<float>(mouseX) / width , static_cast<float>(mouseY) / height);
    glEnableVertexAttribArray(0);
    glDrawArrays(GL_POINTS, 0, nbParticles);
    glDisableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void    Renderer::createProgram()
{
    pId = glCreateProgram();
    addShader(GL_VERTEX_SHADER, fileToString(getCurrentDir() + "/shader.vsh"), &vshId);
    addShader(GL_FRAGMENT_SHADER, fileToString(getCurrentDir() + "/shader.fsh"), &fshId);
    glAttachShader(pId, vshId);
    glAttachShader(pId, fshId);
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
    lock = false;
    mat_pers = new float[16];
    mat_pers[0] = 1.0f / (((float)width / (float)height) * \
                          tan(180.0f / 2.0f));
    mat_pers[1] = 0.0f;
    mat_pers[2] = 0.0f;
    mat_pers[3] = 0.0f;
    mat_pers[4] = 0.0f;
    mat_pers[5] = 1.0f / tan(180.0f / 2.0f) + 1.0f;
    mat_pers[6] = 0.0f;
    mat_pers[7] = 0.0f;
    mat_pers[8] = 0.0f;
    mat_pers[9] = 0.0f;
    mat_pers[10] = (0.2f - 1.0f) / (-0.2f - 1.0f);
    mat_pers[11] = (2.0f * -0.2f * 1.0f) / (-0.2f - 1.0f);
    mat_pers[12] = 0.0f;
    mat_pers[13] = 0.0f;
    mat_pers[14] = 1.0f;
    mat_pers[15] = 0.7f;
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
    clObject->createKernel("compute", nbParticles);
    
}

Renderer::Renderer(int particlesNb, int width, int height)
{
    init(particlesNb, width, height);
}

Renderer::Renderer()
{
    init(1000000, 800, 600);
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
    glDeleteBuffers(1, &vboId);
}
