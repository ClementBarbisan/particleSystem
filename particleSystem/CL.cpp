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
#include <cmath>
#include "utils.hpp"

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

void    CL::compute()
{
    clEnqueueAcquireGLObjects(queue, 1, &clBuffer, 0, NULL, NULL);
    clFinish(queue);
    
    err = clEnqueueNDRangeKernel(queue, kernel, 3, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    clFinish(queue);
    clEnqueueReleaseGLObjects(queue, 1, &clBuffer, 0, NULL, NULL);
    clFinish(queue);
    if (err != CL_SUCCESS)
    {
        std::cout << err << std::endl;
        if (err == CL_OUT_OF_RESOURCES)
            throw std::runtime_error ("Failed to execute kernel : OUT_OF_RESOUCES");
        else if (err == CL_INVALID_WORK_GROUP_SIZE)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_WORK_GROUP_SIZE");
        else if (err == CL_OUT_OF_HOST_MEMORY)
            throw std::runtime_error ("Failed to execute kernel : CL_OUT_OF_HOST_MEMORY");
        else if (err == CL_MEM_OBJECT_ALLOCATION_FAILURE)
            throw std::runtime_error ("Failed to execute kernel : CL_MEM_OBJECT_ALLOCATION_FAILURE");
        else if (err == CL_INVALID_PROGRAM_EXECUTABLE)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_PROGRAM_EXECUTABLEE");
        else if (err == CL_INVALID_COMMAND_QUEUE)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_COMMAND_QUEUE");
        else if (err == CL_INVALID_KERNEL)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_KERNEL");
        else if (err == CL_INVALID_CONTEXT)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_CONTEXT");
        else if (err == CL_INVALID_WORK_ITEM_SIZE)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_WORK_ITEM_SIZE");
        else if (err == CL_INVALID_KERNEL_ARGS)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_KERNEL_ARGS");
        else if (err == CL_INVALID_GLOBAL_OFFSET)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_GLOBAL_OFFSET");
        else if (err == CL_INVALID_EVENT_WAIT_LIST)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_EVENT_WAIT_LIST");
        else if (err == CL_INVALID_WORK_DIMENSION)
            throw std::runtime_error ("Failed to execute kernel : CL_INVALID_WORK_DIMENSION");
        else
            throw std::runtime_error ("Failed to execute kernel : unknown");
    }
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

void    CL::calculateWorkSize()
{
    size_t tmp = sqrt(particules);
    size_t multiple = findMultiple(tmp, 64);
    tmp /= multiple;
    multiple *= multiple;
    globalWorkSize = new size_t[3];
    localWorkSize = new size_t[3];
    globalWorkSize[0] = tmp;
    globalWorkSize[1] = tmp;
    globalWorkSize[2] = multiple;
    std::cout << "globalWorkSize = " << tmp << ", " << tmp << ", " << multiple << std::endl;
    size_t localMultiple = findMultiple(tmp, 64);
    localWorkSize[0] = localMultiple;
    localWorkSize[1] = localMultiple;
    size_t currentMultiple = findMultiple(multiple, 64);
    while (localMultiple * localMultiple * currentMultiple > 1024 && currentMultiple > 1)
        currentMultiple = findMultiple(multiple, currentMultiple - 1);
    localWorkSize[2] = currentMultiple;
    std::cout << "localMultiple = " << localMultiple << ", " << localMultiple << ", " << currentMultiple << std::endl;
}

void    CL::createKernel(std::string name, int nbParticule)
{
    size_t size;
    kernel = clCreateKernel(program, name.c_str(), &err);
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &clBuffer);
    clGetKernelWorkGroupInfo(kernel, devices[0], CL_KERNEL_WORK_GROUP_SIZE, 0, NULL, &size);
    clGetKernelWorkGroupInfo(kernel, devices[0], CL_KERNEL_WORK_GROUP_SIZE, size, &kernelGroupSize, 0);
    std::cout << "kernelWorkGroupSize = " << kernelGroupSize << std::endl;
    particules = nbParticule;
    calculateWorkSize();
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Failed to create kernel");
}

cl_kernel    CL::getKernel()
{
    return (kernel);
}

void    CL::getDeviceInfo()
{
    size_t size;
    clGetDeviceInfo(devices[0], CL_DEVICE_MAX_COMPUTE_UNITS, 0, NULL, &size);
    clGetDeviceInfo(devices[0], CL_DEVICE_MAX_COMPUTE_UNITS, size, &computeUnits, NULL);
    std::cout << "computeUnits = " << computeUnits << std::endl;
    clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_GROUP_SIZE, 0, NULL, &size);
    clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_GROUP_SIZE, size, &workGroupSize, 0);
    std::cout << "workGroupSize = " << workGroupSize << std::endl;
    clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, 0, NULL, &size);
    clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, size, &workItemDimensions, 0);
    std::cout << "workItemsDimensions = " << workItemDimensions << std::endl;
    size_t workItems[workItemDimensions];
    clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workItems), &workItems, 0);
    std::cout << "workItemsSize = { " << workItems[0];
    for (int i = 1; i < workItemDimensions; i++)
        std::cout << ", " << workItems[i];
    std::cout << "}" << std::endl;
    workItemsSize = workItems;
}

CL::CL()
{
    createPlatform();
    getDevices();
    createContext();
    createCommandQueue();
    getDeviceInfo();
}

CL::~CL()
{
    delete devices;
}

