////////////////////////////////////////////////////////////////////////////////
// File:        utility.cc
// Class:       MAT320
// Asssignment: Project 2
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-10-12
////////////////////////////////////////////////////////////////////////////////

#include <fstream>

#include "utility.h"

bool ReadFile(std::vector<std::string> * content, std::string filename)
{
    std::ifstream file(filename.c_str());
    if(!file.is_open())
    {
        return false;
    }
    std::string new_line;
    while(std::getline(file, new_line))
    {
        content->push_back(new_line);
    }
    return true;
}

bool IsWhitespace(char c)
{
    return (c == '\r' || c == '\n' || c == ' ' || c == '\t');
}

std::string StripWhitespace(const std::string & string)
{
    std::string no_whitespace;
    for(char c : string)
    {
        if(!IsWhitespace(c))
        {
            no_whitespace.push_back(c);
        }
    }
    return no_whitespace;
}

