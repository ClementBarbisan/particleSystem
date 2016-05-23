//
//  main.cpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/13/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#include <iostream>
#include "main.h"
#include "utils.hpp"
#include <GLFW/glfw3.h>

void    initglfw()
{
    glfwInit();
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_FLOATING, GL_TRUE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void    init()
{
    toExit = false;
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable( GL_DEPTH_TEST );
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    renderer = new Renderer();
    
}

void	callback_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    toExit = true;
}

void    render()
{
    renderer->render();
}

void    mainLoop(GLFWwindow *window)
{
    while (!toExit)
    {
        render();
        if ((err = glGetError()) != GL_NO_ERROR)
            throw std::runtime_error(searchError(err));
        glfwPollEvents();
        glfwSwapBuffers(window);
    }
}

int main(int argc, const char * argv[])
{
    GLFWwindow  *window;
    
    initglfw();
    if ((window = glfwCreateWindow(WIDTH, HEIGHT, "Particle system", nullptr, nullptr)))
    {
        glfwMakeContextCurrent(window);
        glfwSetWindowPos(window, 50, 50);
        glfwShowWindow(window);
        try
        {
            init();
            glfwSetKeyCallback(window, callback_key);
            mainLoop(window);
        }
        catch (const std::exception &e)
        {
            std::cout << "Error on initialization. " << e.what() << std::endl;
        }
    }
    else
        std::cout << "Can't create window." << std::endl;
    return (0);
}
