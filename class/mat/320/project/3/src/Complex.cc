////////////////////////////////////////////////////////////////////////////////
// File:        Complex.cc
// Class:       MAT320
// Asssignment: Project 3
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-11-04
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <cstdlib>
#include <iostream>

#include "utility.h"

#include "Complex.h"

Complex::Complex() : m_Real(0), m_Imaginary(0)
{}

Complex::Complex(double real, double imaginary)
{
    m_Real = real;
    m_Imaginary = imaginary;
}

Complex::Complex(const Complex & other)
{
    m_Real = other.m_Real;
    m_Imaginary = other.m_Imaginary;
}

void Complex::Polar(double magnitude, double angle)
{
    m_Real = magnitude * std::cos(angle);
    m_Imaginary = magnitude * std::sin(angle);
}

void Complex::Random(int min, int max)
{
    int range = max - min;
    m_Real = double(rand() % range + min);
    m_Imaginary = rand() % range + min;
}

double Complex::Real()
{
    return m_Real;
}

double Complex::Imaginary()
{
    return m_Imaginary;
}

Complex Complex::Conjugate()
{
    return Complex(m_Real, -m_Imaginary);
}

Complex Complex::operator+(const Complex & rhs)
{
    double new_real = m_Real + rhs.m_Real;
    double new_im = m_Imaginary + rhs.m_Imaginary;
    return Complex(new_real, new_im);
}

Complex Complex::operator-(const Complex & rhs)
{
    double new_real = m_Real - rhs.m_Real;
    double new_im = m_Imaginary - rhs.m_Imaginary;
    return Complex(new_real, new_im);
}

Complex Complex::operator*(const Complex & rhs) const
{
    double new_real = m_Real * rhs.m_Real - m_Imaginary * rhs.m_Imaginary;
    double new_im = m_Real * rhs.m_Imaginary + m_Imaginary * rhs.m_Real;
    return Complex(new_real, new_im);
}

Complex Complex::operator*(double rhs) const
{
    double new_real = m_Real * rhs;
    double new_im = m_Imaginary * rhs;
    return Complex(new_real, new_im);
}

Complex Complex::operator/(double rhs) const
{
    double new_real = m_Real / rhs;
    double new_im = m_Imaginary / rhs;
    return Complex(new_real, new_im);
}

Complex & Complex::operator=(const Complex & rhs)
{
    m_Real = rhs.m_Real;
    m_Imaginary = rhs.m_Imaginary;
    return *this;
}

Complex & Complex::operator+=(const Complex & rhs)
{
    m_Real += rhs.m_Real;
    m_Imaginary += rhs.m_Imaginary;
    return *this;
}

Complex & Complex::operator-=(const Complex & rhs)
{
    m_Real -= rhs.m_Real;
    m_Imaginary -= rhs.m_Imaginary;
    return *this;
}

Complex & Complex::operator*=(const Complex & rhs)
{
    double new_real = m_Real * rhs.m_Real - m_Imaginary * rhs.m_Imaginary;
    m_Imaginary = m_Real * rhs.m_Imaginary + m_Imaginary * rhs.m_Real;
    m_Real = new_real;
    return *this;
}

Complex & Complex::operator*=(double rhs)
{
    m_Real *= rhs;
    m_Imaginary *= rhs;
    return *this;
}

bool Complex::StringToComplex(Complex * complex_value, 
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

bool Complex::VStringToVComplex(std::vector<Complex> * v_complex,
                                const std::vector<std::string> & v_string)
{
    for(const std::string & string_value : v_string)
    {
        Complex new_complex;
        bool success = StringToComplex(&new_complex, string_value); 
        if(success)
        {
            v_complex->push_back(new_complex);
        }
        else
        {
            return false;
        }
    }
}

bool Complex::FileToVComplex(std::vector<Complex> * v_complex,
                             const std::string & filename)
{
    std::vector<std::string> string_values;
    bool success = ReadFile(&string_values, filename);
    if(!success)
    {
        std::cout << "Error: " << filename << " not found." << std::endl;
        return false;
    }
    VStringToVComplex(v_complex, string_values);
    return true;
}

std::ostream & operator<<(std::ostream & os, const Complex & value)
{
    os << value.m_Real;
    if(value.m_Imaginary >= 0.0f)
    {
        os << " + ";
    }
    else
    {
        os << " - ";
    }
    double imaginary_positive = std::abs(value.m_Imaginary);
    os << imaginary_positive << 'i';
    return os;
}

