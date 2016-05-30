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
    context = clCreateContext(properties, 1, devices, NULL, NULL, &err);
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
        throw std::runtime_error ("Failed to get number of devices available");
    devices = new cl_device_id[nbDevice];
    if ((err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, nbDevice, devices, NULL)) != CL_SUCCESS)
            throw std::runtime_error ("Failed to get device IDs.");
	if (nbDevice > 1)
		devices[0] = devices[1];
}

void    CL::createCommandQueue()
{
    queue = clCreateCommandQueue(context, devices[0], 0, NULL);
}

void    CL::compute(int indexK)
{
    clEnqueueAcquireGLObjects(queue, 1, &clBuffer, 0, NULL, NULL);
    clFinish(queue);
    
    err = clEnqueueNDRangeKernel(queue, kernel[indexK], 3, NULL, globalWorkSize, localWorkSize, 0, NULL, NULL);
    clFinish(queue);
    clEnqueueReleaseGLObjects(queue, 1, &clBuffer, 0, NULL, NULL);
    clFinish(queue);
    if (err != CL_SUCCESS)
		throw std::runtime_error ("Failed to execute kernel : " + std::to_string(err));
}

void    CL::createProgram(std::string filename)
{
    std::ifstream ifs(filename);
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    const char *file = content.c_str();
    program = clCreateProgramWithSource(context, 1, &file, NULL, &err);
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Failed to create program from source.");
    if ((err = clBuildProgram(program, 1, devices, NULL, NULL, NULL)) != CL_SUCCESS)
        throw std::runtime_error ("Failed to build program : "  + std::to_string(err));
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
    size_t multiple = findMultiple(tmp, 8);
    tmp /= multiple;
    multiple *= multiple;
    globalWorkSize = new size_t[3];
    localWorkSize = new size_t[3];
    globalWorkSize[0] = tmp;
    globalWorkSize[1] = tmp;
    globalWorkSize[2] = multiple;
    std::cout << "globalWorkSize = " << tmp << ", " << tmp << ", " << multiple << std::endl;
    totalWorkSize = tmp * tmp * multiple;
    size_t localMultiple = findMultiple(tmp, 32);
    localWorkSize[0] = localMultiple;
    localWorkSize[1] = localMultiple;
    size_t workItems[workItemDimensions];
    clGetDeviceInfo(devices[0], CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(workItems), &workItems, 0);
    size_t currentMultiple = findMultiple(multiple, workItems[workItemDimensions - 1]);
    while (localMultiple * localMultiple * currentMultiple > workGroupSize && currentMultiple > 1)
        currentMultiple = findMultiple(multiple, currentMultiple - 1);
    localWorkSize[2] = currentMultiple;
    std::cout << "localMultiple = " << localMultiple << ", " << localMultiple << ", " << currentMultiple << std::endl;
}

size_t  CL::getTotalWorkSize()
{
    return (totalWorkSize);
}

void    CL::createKernel(std::string name, int nbParticule)
{
    size_t size;
    kernel[index] = clCreateKernel(program, name.c_str(), &err);
	if (err != CL_SUCCESS)
		throw std::runtime_error ("Failed to create kernel : " + std::to_string(err));
    err = clSetKernelArg(kernel[index], 0, sizeof(cl_mem), &clBuffer);
    clGetKernelWorkGroupInfo(kernel[index], devices[0], CL_KERNEL_WORK_GROUP_SIZE, 0, NULL, &size);
    clGetKernelWorkGroupInfo(kernel[index], devices[0], CL_KERNEL_WORK_GROUP_SIZE, size, &kernelGroupSize, 0);
    std::cout << "kernelWorkGroupSize = " << kernelGroupSize << std::endl;
    particules = nbParticule;
    calculateWorkSize();
    index++;
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Failed to create kernel : "  + std::to_string(err));
}

cl_kernel    CL::getKernel(int i)
{
    return (kernel[i]);
}

cl_context  CL::getContext()
{
    return (context);
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
    if (workItemDimensions < 3)
        throw std::runtime_error ("Not enough work dimensions on this device");
}

CL::CL()
{
    kernel = new cl_kernel[4];
    index = 0;
    createPlatform();
    getDevices();
    createContext();
    createCommandQueue();
    getDeviceInfo();
}

CL::~CL()
{
    clReleaseDevice(devices[0]);
    for (int i = 0; i < index; i++)
        clReleaseKernel(kernel[i]);
    clReleaseContext(context);
    clReleaseProgram(program);
    clReleaseMemObject(clBuffer);
    clReleaseCommandQueue(queue);
    delete devices;
    delete kernel;
}

