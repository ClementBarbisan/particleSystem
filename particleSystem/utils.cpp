//
//  utils.cpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/23/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#include "utils.hpp"
#include <unistd.h>
#include <sstream>

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

float           dot(const float *vec1,const float *vec2)
{
    return (vec1[0] * vec2[0] + vec1[1] * vec2[1] + vec1[2] * vec2[2]);
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

int             stringToInt(const char *str)
{
    std::string hello(str);
    std::stringstream string(hello);
    int x;
    string >> x;
    if (!str)
        throw std::logic_error(std::string(str) + " : Not an int");
    return (x);
}