//
//  utils.cpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/23/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#include "utils.hpp"
#include <unistd.h>

std::string    searchError(GLuint error)
{
    if (error == GL_INVALID_ENUM)
        return ("GL_Error : Invalid_enum");
    else if (error == GL_INVALID_VALUE)
        return ("GL_Error : Invalid_value");
    else if (error == GL_INVALID_OPERATION)
        return ("GL_Error : Invalid_operation");
    return ("GL_Error");
}

std::string    fileToString(std::string filename)
{
    std::ifstream       file(filename);
    std::stringstream   buffer;
    if (file)
    {
        buffer << file.rdbuf();
        file.close();
    }
    else
        throw std::runtime_error("Failed to open : " + filename);
    return (buffer.str());
}

std::string    getCurrentDir()
{
    char    currentPath[128];
    
    if (!getcwd(currentPath, 128))
        throw std::runtime_error("Failed to find current directory.");
    return (std::string(currentPath));
}

size_t          findMultiple(size_t value, size_t div)
{
    while (div > 1 && value % div)
        div--;
    return (div);
}