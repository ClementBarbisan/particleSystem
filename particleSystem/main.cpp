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

void    init(int nb)
{
    toExit = false;
    onClick = false;
    mouseY = 0;
    mouseX = 0;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glViewport(0, 0, WIDTH, HEIGHT);
    glEnable( GL_DEPTH_TEST );
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    renderer = new Renderer(nb, WIDTH, HEIGHT);
}

void	callbackKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (window && action == GLFW_PRESS)
    {
        if (key == 256)
             toExit = true;
        else if (key == 32)
            renderer->changeShape();
    }
}

void    onClickButton(GLFWwindow *window, int button, int action, int mods)
{
    if (button == 0)
    {
        if (action == GLFW_PRESS)
            onClick = true;
        else
            onClick = false;
    }
}

void    onMouseMove(GLFWwindow *window, double x, double y)
{
    mouseX = x;
    mouseY = y;
}

void    render()
{
    renderer->render(mouseX, mouseY, onClick);
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
    if (argc > 1 && (window = glfwCreateWindow(WIDTH, HEIGHT, "Particle system", nullptr, nullptr)))
    {
        glfwMakeContextCurrent(window);
        glfwSetWindowPos(window, 50, 50);
        glfwShowWindow(window);
        try
        {
            init(stringToInt(argv[1]));
            glfwSetKeyCallback(window, callbackKey);
            glfwSetCursorPosCallback(window, onMouseMove);
            glfwSetMouseButtonCallback(window, onClickButton);
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
