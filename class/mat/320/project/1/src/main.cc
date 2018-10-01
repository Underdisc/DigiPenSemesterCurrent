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

bool read_file(std::vector<std::string> * content, std::string filename)
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
    unsigned split_index = 0;
    unsigned end_index = 0;
    unsigned char complete = 0;
    std::string stripped_string = strip_whitespace(string_value);
    for(unsigned i = 0; i < stripped_string.size(); ++i)
    {
        int c = (int)stripped_string[i];
        if(c == '+' || c == '-')
        {
            split_index = i; 
            complete |= 1;
        }
        if(c == 'i')
        {
            end_index = i;
            complete |= 2;
        }
    }
    if(complete != 3)
    {
        return false;
    }

    std::string r_string = stripped_string.substr(0, split_index);
    std::string i_string = stripped_string.substr(split_index, 
                                                  end_index - split_index);

    complex_value->m_Real = std::stod(r_string);
    complex_value->m_Imaginary = std::stod(i_string);

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
        std::cout << "Error: " << filename << " not found." << std::endl;
        return false;
    }
    parse_complex_values(complex_values, string_values);
    return true;
}

void Part1(const std::vector<std::string> & args)
{
    if(args.size() != 4)
    {
        std::cout << "Incorrect number of arguments provided." << std::endl;
        return;
    }
    const std::string & filename = args[3];
    std::vector<Complex> complex_values;
    bool success = read_complex_values(&complex_values, filename);
    if(!success)
    {
        return;
    }

    Complex rotation;
    double x = std::stod(args[2]);
    rotation.Polar(1.0, 2.0 * PI * x); 

    unsigned n = (unsigned)std::stoul(args[1]);
    for(unsigned i = 0; i < n; ++i)
    {
        Complex new_complex;
        new_complex = complex_values[i] * rotation;
        std::cout << new_complex << std::endl;
    }
}

void Part2(const std::vector<std::string> & args)
{
    if(args.size() != 3)
    {
        std::cout << "Incorrect number of arguments provided." << std::endl;
        return;
    }

    double n = std::stod(args[1]);
    int k = std::stoi(args[2]);

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
    if(args.size() != 4)
    {
        std::cout << "Incorrect number of arguments provided." << std::endl;
        return;
    }

    unsigned n = (unsigned)std::stol(args[1]);
    const std::string & lhs_filename = args[2];
    const std::string & rhs_filename = args[3];

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
    unsigned n = std::stol(args[1]);
    const std::string & filename = args[2];

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

int main(int argc, char ** argv)
{
    if(argc < 2)
    {
        std::cout << "A test number [1-4] must be specified as the second "
                     "argument."
                  << std::endl;
        return 0;
    }

    std::vector<std::string> arguments;
    for(int i = 1; i < argc; ++i)
    {
        arguments.push_back(argv[i]);
    }

    if(argv[1][0] == (char)'1')
    {
        Part1(arguments);
    }
    else if(argv[1][0] == (char)'2')
    {
        Part2(arguments);
    }
    else if(argv[1][0] == (char)'3')
    {
        Part3(arguments);
    }
    else if(argv[1][0] == (char)'4')
    {
        Part4(arguments);
    }
}
