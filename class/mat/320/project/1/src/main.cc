////////////////////////////////////////////////////////////////////////////////
// File:        main.cc
// Class:       MAT320
// Asssignment: Project 1
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-09-28
////////////////////////////////////////////////////////////////////////////////

// Usage
// ./exe 1 N x filename
// ./exe 2 N k
// ./exe 3 N filename filename
// ./exe 4 N filename

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Complex.h"

#define PI 3.14159265358979323846

// Utility Functions ///////////////////////////////////////////////////////////

bool read_file(std::vector<std::string> * content, std::string filename)
{
    std::ifstream file(filename.c_str());
    if(!file.is_open())
    {
        std::cout << "Error: File \"" << filename << "\" not found." 
                  << std::endl;
        return false;
    }
    std::string new_line;
    while(std::getline(file, new_line))
    {
        content->push_back(new_line);
    }
    return true;
}

bool is_whitespace(char c)
{
    return (c == '\r' || c == '\n' || c == ' ' || c == '\t');
}

std::string strip_whitespace(const std::string & string)
{
    std::string no_whitespace;
    for(char c : string)
    {
        if(!is_whitespace(c))
        {
            no_whitespace.push_back(c);
        }
    }
    return no_whitespace;
}

bool string_to_complex(Complex * complex_value, 
                       const std::string & string_value)
{
    std::vector<double> reals;
    std::vector<double> imags;
    std::string number;
    double unary = 1.0;
    unsigned state = 0;
    for(int i = 0; i < string_value.size(); ++i)
    { 
        char c = string_value[i];
        switch(state)
        {
        case 0:
            number.clear();
            unary = 1.0;
            if(c == '-')
            {
                unary *= -1.0;
                state = 1;
            }
            else if(c == '+')
            {
                state = 1;
            }
            else if(c >= '0' && c <= '9')
            {
                number.push_back(c);
                state = 2;
            }
            else if(c == 'i')
            {
                state = 3;
            }
            else
            {
                state = 0;
            }
            break;

        case 1:
            if(c == ' ')
            {
                state = 1;
            }
            else if(c == '-')
            {
                unary *= -1.0;
                state = 1;
            }
            else if(c == '+')
            {
                state = 1;
            }
            else if(c == 'i')
            {
                state = 3;
            }
            else if(c >= '0' && c <= '9')
            {
                number.push_back(c);
                state = 2;
            }
            else
            {
                --i;
                state = 0;
            }
            break;

        case 2:
            if(c >= '0' && c <= '9')
            {
                number.push_back(c);
                state = 2;
            }
            else if(c == 'i')
            {
                state = 4;
            }
            else
            {
                reals.push_back(unary * std::stod(number));
                --i;
                state = 0;
            }
            break;

        case 3:
            if(c >= '0' && c <= '9')
            {
                number.push_back(c);
                state = 5;
            }
            else
            {
                number.push_back('1');
                imags.push_back(unary * std::stod(number));
                --i;
                state = 0;
            }
            break;

        case 4:
            imags.push_back(unary * std::stod(number));
            --i;
            state = 0;
            break;

        case 5:
            if(c >= '0' && c <= '9')
            {
                number.push_back(c);
                state = 5;
            }
            else
            {
                imags.push_back(unary * std::stod(number));
                --i;
                state = 0;
            }
            break;
        }
    }

    if(state == 2)
    {
        reals.push_back(unary * std::stod(number));
    }
    else if(state == 4 || state == 5)
    {
        imags.push_back(unary * std::stod(number));
    }
    else if(state == 3)
    {
        number.push_back('1');
        imags.push_back(unary * std::stod(number));
    }

    complex_value->m_Real = 0;
    for(double value : reals)
    {
        complex_value->m_Real += value;
    }
    complex_value->m_Imaginary = 0;
    for(double value : imags)
    {
        complex_value->m_Imaginary += value;
    }
    return true;
}

void parse_complex_values(std::vector<Complex> * complex_values,
                          const std::vector<std::string> & string_values)
{
    for(const std::string & string_value : string_values)
    {
        Complex new_complex;
        bool success = string_to_complex(&new_complex, string_value); 
        if(success)
        {
            complex_values->push_back(new_complex);
        }
    }
}

bool read_complex_values(std::vector<Complex> * complex_values,
                         const std::string filename)
{
    std::vector<std::string> string_values;
    bool success = read_file(&string_values, filename);
    if(!success)
    {
        return false;
    }
    parse_complex_values(complex_values, string_values);
    return true;
}

// Project Functions ///////////////////////////////////////////////////////////

void Part1(const std::vector<std::string> & args)
{
    const std::string & filename = args[2];
    std::vector<Complex> complex_values;
    bool success = read_complex_values(&complex_values, filename);
    if(!success)
    {
        return;
    }

    Complex rotation;
    double x = std::stod(args[1]);
    rotation.Polar(1.0, 2.0 * PI * x); 

    unsigned n = (unsigned)std::stoul(args[0]);
    for(unsigned i = 0; i < n; ++i)
    {
        Complex new_complex;
        new_complex = complex_values[i] * rotation;
        std::cout << new_complex << std::endl;
    }
}

void Part2(const std::vector<std::string> & args)
{
    double n = std::stod(args[0]);
    int k = std::stoi(args[1]);

    Complex output(0.0, 0.0);
    for(int i = 0; i < k; ++i)
    {
        Complex term;
        double root = (double)i;
        term.Polar(1.0, root * (2.0 * PI) / n);
        output += term; 
    }

    std::cout << output << std::endl;
}

void Part3(const std::vector<std::string> & args)
{
    unsigned n = (unsigned)std::stol(args[0]);
    const std::string & lhs_filename = args[1];
    const std::string & rhs_filename = args[2];

    std::vector<Complex> lhs_complex_values;
    std::vector<Complex> rhs_complex_values;

    bool success = read_complex_values(&lhs_complex_values, lhs_filename);
    if(!success)
    {
        return;
    }
    success = read_complex_values(&rhs_complex_values, rhs_filename);
    if(!success)
    {
        return;
    }

    Complex output(0.0, 0.0);
    for(unsigned i = 0; i < n; ++i)
    {
        output += lhs_complex_values[i] * rhs_complex_values[i].Conjugate();
    }
    std::cout << output << std::endl;
}

void Part4(const std::vector<std::string> & args)
{
    unsigned n = std::stol(args[0]);
    const std::string & filename = args[1];

    std::vector<Complex> lhs_complex_values;
    std::vector<Complex> rhs_complex_values;

    bool success = read_complex_values(&lhs_complex_values, filename);
    if(!success)
    {
        return;
    }

    for(unsigned i = 0; i < n; ++i)
    {
        Complex root_of_unity;
        double n_double = (double)n;
        root_of_unity.Polar(1.0, (double)i * 2.0 * PI / n_double);
        rhs_complex_values.push_back(root_of_unity);
    }

    Complex output(0.0, 0.0);
    for(unsigned i = 0; i < n; ++i)
    {
        output += lhs_complex_values[i] * rhs_complex_values[i].Conjugate();
    }
    std::cout << output << std::endl;
}

// Main ////////////////////////////////////////////////////////////////////////

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        std::cout << "Error: A test number [1-4] must be specified as the "
                     "second argument."
                  << std::endl;
        return 0;
    }

    std::vector<std::string> arguments;
    for(int i = 2; i < argc; ++i)
    {
        arguments.push_back(argv[i]);
    }

    if(argv[1][0] == (char)'1')
    {
        if(arguments.size() != 3)
        {
            std::cout << "Error: Part 1 takes 3 arguments." << std::endl 
                      << "./prog.exe 1 N x filename" << std::endl;
            return 0;
        }
        Part1(arguments);
    }
    else if(argv[1][0] == (char)'2')
    {
        if(arguments.size() != 2)
        {
            std::cout << "Error: Part 2 takes 2 arguments." << std::endl 
                      << "./prog.exe 2 N k" << std::endl;
            return 0;
        }
        Part2(arguments);
    }
    else if(argv[1][0] == (char)'3')
    {
        if(arguments.size() != 3)
        {
            std::cout << "Error: Part 3 takes 3 arguments." << std::endl 
                      << "./prog.exe 3 N filename filename" << std::endl;
            return 0;
        }
        Part3(arguments);
    }
    else if(argv[1][0] == (char)'4')
    {
        if(arguments.size() != 2)
        {
            std::cout << "Error: Part 4 takes 2 arguments." << std::endl 
                      << "./prog.exe 4 N filename" << std::endl;
            return 0;
        }
        Part4(arguments);
    }
}
