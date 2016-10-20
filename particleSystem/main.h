//
//  main.h
//  particleSystem
//
//  Created by Clement BARBISAN on 5/23/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#ifndef main_h
#define main_h
# define __gl_h_
# define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
# include <OpenGL/gl3.h>
#include <OpenGL/gl.h>
#include "Renderer.hpp"
#include "position.h"

#define WIDTH 1024
#define HEIGHT 768

bool toExit;
Renderer *renderer;
int mouseX;
int mouseY;
int oldMouseX;
int oldMouseY;
bool onClick;
GLuint err;

#endif /* main_h */
