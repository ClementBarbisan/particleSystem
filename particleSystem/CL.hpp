//
//  CL.hpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/17/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#ifndef CL_hpp
#define CL_hpp

#include <OpenCL/Opencl.h>
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/gl.h>
#include <OpenGL/OpenGL.h>
#include <stdio.h>
#include <string>

class CL
{
    private:
        cl_platform_id      platform;
        cl_device_id        *devices;
        cl_uint             nbDevice;
        cl_context          context;
        cl_command_queue    queue;
        cl_program          program;
        cl_kernel           *kernel;
        cl_int              err;
        cl_mem              clBuffer;
        void                createContext();
        void                createPlatform();
        void                getDevices();
        void                createCommandQueue();
        void                getDeviceInfo();
        size_t              workGroupSize;
        cl_uint             workItemDimensions;
        size_t              *workItemsSize;
        cl_uint             computeUnits;
        size_t              kernelGroupSize;
        size_t              *globalWorkSize;
        size_t              *localWorkSize;
        size_t              totalWorkSize;
        int                 particules;
        void                calculateWorkSize();
        int                 index;
    public:
        CL();
        ~CL();
        size_t              getTotalWorkSize();
        void                compute(int indexK);
        void                createProgram(std::string filename);
        void                shareBuffer(GLuint vboId);
        void                createKernel(std::string name, int nbParticule);
        cl_kernel           getKernel(int i);
        int                 num;
    
};

#endif /* CL_hpp */
