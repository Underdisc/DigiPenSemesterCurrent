////////////////////////////////////////////////////////////////////////////////
// File:        Complex.h
// Class:       MAT320
// Asssignment: Project 3
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-11-04
////////////////////////////////////////////////////////////////////////////////

#ifndef COMPLEX_H
#define COMPLEX_H

#include <string>
#include <vector>

class Complex
{
public:
    Complex();
    Complex(double real, double imaginary);
    Complex(const Complex & other);
    void Polar(double magnitude, double angle);
    void Random(int min, int max);
    double Real();
    double Imaginary();
    Complex Conjugate();
    Complex operator+(const Complex & rhs);
    Complex operator-(const Complex & rhs);
    Complex operator*(const Complex & rhs) const;
    Complex operator*(double rhs) const;
    Complex operator/(double rhs) const;
    Complex & operator=(const Complex & rhs);
    Complex & operator+=(const Complex & rhs);
    Complex & operator-=(const Complex & rhs);
    Complex & operator*=(const Complex & rhs);
    Complex & operator*=(double rhs);
    union
    {
      struct
      {
        double m_Real, m_Imaginary;
      };
      double m_Values[2];
    };
    static bool StringToComplex(Complex * complex_value,
                                const std::string & string_value);
    static bool VStringToVComplex(std::vector<Complex> * v_complex,
                                  const std::vector<std::string> & v_string);
    static bool FileToVComplex(std::vector<Complex> * v_complex,
                               const std::string & filename);
    friend std::ostream & operator<<(std::ostream & os, const Complex & value);
};

#endif
