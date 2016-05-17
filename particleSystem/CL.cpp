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

void      CL::create_context()
{
    cl_context_properties context_properties[] = {
        CL_CONTEXT_PROPERTIES,
        reinterpret_cast<cl_context_properties>(platform),
        0
    };
    context = clCreateContextFromType(context_properties, CL_DEVICE_TYPE_GPU, NULL, NULL, &err);
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Can't create OpenCL context for GPU.");
}

void  CL::create_platform()
{
    cl_uint nb_platforms = 0;
    
    if (clGetPlatformIDs(1, &platform, &nb_platforms) != CL_SUCCESS || nb_platforms <= 0)
        throw std::runtime_error ("No OpenCL platform detected.");
}

void    CL::get_devices()
{
    if ((clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 0, NULL, &nb_device)) != CL_SUCCESS || nb_device <= 0)
        throw std::runtime_error ("Failde to get number of devices available");
    if (clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, nb_device, devices, NULL) != CL_SUCCESS)
        throw std::runtime_error ("Failed to get device IDs.");
}

void    CL::create_command_queue()
{
    queue = clCreateCommandQueue(context, devices[0], 0, NULL);
}

void    CL::create_program(std::string filename)
{
    std::ifstream ifs(filename);
    std::string content((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
    const char *file = content.c_str();
    program = clCreateProgramWithSource(context, 1, &file, NULL, &err);
    if (err != CL_SUCCESS)
        throw std::runtime_error ("Failed to create program from source.");
    if (clBuildProgram(program, nb_device, devices, NULL, NULL, NULL) != CL_SUCCESS)
        throw std::runtime_error ("Failed to build program");
}

CL::CL()
{
    create_platform();
    create_context();
    get_devices();
    create_command_queue();
}

CL::~CL()
{
    
}

