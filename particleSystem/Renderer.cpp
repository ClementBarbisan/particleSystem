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

void    Renderer::createParticles(int nb)
{
    int floatByteSize = 4;
    int positionFloatCount = 3;
    int textureFloatCount = 0;
    int normalFloatCount = 0;
    int floatsPerVertex = positionFloatCount + textureFloatCount + normalFloatCount;
    int vertexFloatSizeInBytes = floatByteSize * floatsPerVertex;
    
    glGenVertexArrays(1, &vaoId);
    glGenBuffers(1, &vboId);
    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER, nb * sizeof(Particle), NULL, GL_STATIC_DRAW);
    Particle *particles = (Particle *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    for (int i; i < nb; i++)
    {
        particles->x = 0.5;
        particles->y = 0.5;
        particles->z = -1.0;
        particles++;
    }
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glVertexAttribPointer(0, positionFloatCount, GL_FLOAT, GL_FALSE, vertexFloatSizeInBytes, 0);
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

void    Renderer::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(pId);
    glBindVertexArray(vaoId);
    glUniformMatrix4fv(glGetUniformLocation(pId, "projectionMatrix"), 1, GL_FALSE, matrix);
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
    glAttachShader(pId, vshId);
    glAttachShader(pId, fshId);
    glBindAttribLocation(pId, 0, "vertexPosition");
    glLinkProgram(pId);
    glValidateProgram(pId);
    if ((err = glGetError()) != GL_NO_ERROR)
        throw std::runtime_error(searchError(err) + " : createProgram.");
}

void    Renderer::init(int nb, int width, int height)
{
    float	*mat_pers;
    
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
    matrix = mat_pers;
    nbParticles = nb;
    createParticles(nb);
    createProgram();
}

Renderer::Renderer(int particlesNb, int width, int height)
{
    init(particlesNb, width, height);
}

Renderer::Renderer()
{
    init(10000, 800, 600);
}