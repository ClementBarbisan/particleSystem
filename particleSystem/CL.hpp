//
//  CL.hpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/17/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#ifndef CL_hpp
#define CL_hpp

#if defined __APPLE__ || defined(MACOSX)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif
#include <stdio.h>
#include <string>

class CL
{
    private:
        cl_platform_id      platform;
        cl_device_id        *devices;
        cl_uint             nb_device;
        cl_context          context;
        cl_command_queue    queue;
        cl_program          program;
        cl_kernel           kernel;
        cl_int              err;
        cl_event            event;
        void    create_context();
        void    create_platform();
        void    get_devices();
        void    create_command_queue();
    public:
        CL();
        ~CL();
        void    create_program(std::string filename);
        int                 num;
    
};

#endif /* CL_hpp */
