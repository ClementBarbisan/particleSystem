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
#include "CL.hpp"
#include <map>
#include "position.h"
#include "trigo.h"

class Renderer
{
    typedef void (Renderer::*funcShape)(void);
    public:
        Renderer();
        Renderer(int particlesNb, int width, int height);
        void        render(int mouseX, int mouseY, bool mouseDown);
        void        changeShape();
		void		changeProgram();
        void        setDelta(float dt);
        t_pos   &   getPosition();
		float	&	getMass();
        ~Renderer();
    private:
        int         nbParticles;
		int			programIndex;
        int         width;
        int         height;
        float       mass;
        void        createParticles(int nb);
        void        createProgram(int shader);
        void        addShader(GLenum typeShader, std::string shader, GLuint *sh);
        void        errorShader(GLuint id);
        void        init(int nb, int width, int height);
        void        sphereShape();
        void        cubeShape();
        void        openclComputation(float scale);
        void        gravityBehaviour();
        void        matrixViewInit();
        void        matrixPers();
        t_trigo     createTrigo();
        void        updateMatrix();
		GLfloat		*identityMatrix();
        void        initPosition();
        float       *matrix;
        float       *matrixView;
		float		*modelMatrix;
        bool        lock;
        float       delta;
        bool        gravity;
        int         currentMouseX;
        int         currentMouseY;
        int         currentShape;
        CL          *clObject;
        funcShape   *mapShapes;
        t_pos       *position;
        GLuint      vaoId;
        GLuint      vboId;
        GLuint      *pId;
        GLuint      gshId;
        GLuint      *vshId;
        GLuint      fshId;
        GLuint      err;
};

#endif /* Renderer_hpp */
