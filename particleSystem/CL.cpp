//
//  CL.cpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/17/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#include "CL.hpp"
#include <iostream>
#include <fstream>

void      CL::createContext()
{
    CGLContextObj kCGLContext = CGLGetCurrentContext();
    CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
    cl_context_properties properties[] = {
        CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
        (cl_context_properties)kCGLShareGroup, 0
    };
    context = clCreateContext(properties, nbDevice, devices, NULL, NULL, &err);
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Can't create OpenCL context for GPU.");
}

void  CL::createPlatform()
{
    cl_uint nb_platforms = 0;
    
    if (clGetPlatformIDs(1, &platform, &nb_platforms) != CL_SUCCESS || nb_platforms <= 0)
        throw std::runtime_error ("No OpenCL platform detected.");
}

void    CL::getDevices()
{
    if ((clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &nbDevice)) != CL_SUCCESS || nbDevice <= 0)
        throw std::runtime_error ("Failde to get number of devices available");
    devices = new cl_device_id[nbDevice];
    if ((err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, nbDevice, devices, NULL)) != CL_SUCCESS)
            throw std::runtime_error ("Failed to get device IDs.");
}

void    CL::createCommandQueue()
{
    queue = clCreateCommandQueue(context, devices[0], 0, NULL);
}

void    CL::compute(int nbParticule)
{
    cl_event event;
    clEnqueueAcquireGLObjects(queue, 1, &clBuffer, 0, NULL, &event);
    clWaitForEvents(1, &event);
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &clBuffer);
    size_t global[1] = {static_cast<size_t>(nbParticule) / 10};
    clEnqueueNDRangeKernel(queue, kernel, 1, NULL, global, NULL, 0, NULL, &event);
    clWaitForEvents(1, &event);
    clEnqueueReleaseGLObjects(queue, 1, &clBuffer, 0, NULL, &event);
    clWaitForEvents(1, &event);
}

void    CL::createProgram(std::string filename)
{
    std::ifstream ifs(filename);
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    const char *file = content.c_str();
    program = clCreateProgramWithSource(context, 1, &file, NULL, &err);
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Failed to create program from source.");
    if ((err = clBuildProgram(program, nbDevice, devices, NULL, NULL, NULL)) != CL_SUCCESS)
        throw std::runtime_error ("Failed to build program");
}

void    CL::shareBuffer(GLuint vboId)
{
    clBuffer = clCreateFromGLBuffer(context, CL_MEM_READ_WRITE, vboId, &err);
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Failed to create buffer from GLbuffer");
}

void    CL::createKernel(std::string name)
{
    kernel = clCreateKernel(program, name.c_str(), &err);
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Failed to create kernel");
}

cl_kernel    CL::getKernel()
{
    return (kernel);
}

CL::CL()
{
    createPlatform();
    getDevices();
    createContext();
    createCommandQueue();
}

CL::~CL()
{
    delete devices;
}

