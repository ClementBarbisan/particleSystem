//
//  Renderer.hpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/23/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#ifndef Renderer_hpp
#define Renderer_hpp
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
# include <OpenGL/gl3.h>
#include <OpenGL/gl.h>
#include <stdio.h>
#include <string>
#include "Particle.h"
#include <map>

class Renderer
{
    typedef void (Renderer::*funcShape)(void);
    public:
        Renderer();
        Renderer(int particlesNb, int width, int height);
        void    render();
        void    changeShape();
        ~Renderer();
    private:
        int     nbParticles;
        void    createParticles(int nb);
        void    createProgram();
        void    addShader(GLenum typeShader, std::string shader, GLuint *sh);
        void    errorShader(GLuint id);
        void    init(int nb, int width, int height);
        void    sphereShape();
        void    cubeShape();
        float   *matrix;
        int     currentShape;
        funcShape *mapShapes;
        GLuint  vaoId;
        GLuint  vboId;
        GLuint  pId;
        GLuint  vshId;
        GLuint  fshId;
        GLuint  err;
};

#endif /* Renderer_hpp */
