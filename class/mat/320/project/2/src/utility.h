////////////////////////////////////////////////////////////////////////////////
// File:        utility.h
// Class:       MAT320
// Asssignment: Project 2
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-10-12
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILITY_H
#define UTILITY_H

#include <string>
#include <vector>

bool ReadFile(std::vector<std::string> * content, std::string filename);
bool IsWhitespace(char c);
std::string StripWhitespace(const std::string & string);

#endif
