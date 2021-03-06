//
//  utils.hpp
//  particleSystem
//
//  Created by Clement BARBISAN on 5/23/16.
//  Copyright © 2016 randomLockNess. All rights reserved.
//

#ifndef utils_hpp
#define utils_hpp

#include <stdio.h>
#include <string>
#include <OpenGL/gl.h>
#include <fstream>
#include <sstream>

std::string     searchError(GLuint error);
std::string     fileToString(std::string filename);
std::string     getCurrentDir();
size_t          findMultiple(size_t value, size_t div);
int             stringToInt(const char *str);
float           dot(const float *vec1,const float *vec2);

#endif /* utils_hpp */
