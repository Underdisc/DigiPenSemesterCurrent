//////////////////////////////////////////////////////////////////////////////
/// @file Complex.h
/// @author Connor Deakin
/// @email connor.deakin@digipen.edu
/// @date 2017-09-21
///
/// @brief Interface and implementation for complex number arithmetic.
///////////////////////////////////////////////////////////////////////////////

#ifndef COMPLEX_H
#define COMPLEX_H

#include <ostream>

class Complex
{
public:
  Complex();
  Complex(double real, double imaginary);
  Complex(const Complex & other);
  void Polar(double magnitude, double angle);
  double Real();
  double Imaginary();
  Complex Conjugate();
  Complex operator+(const Complex & rhs);
  Complex operator-(const Complex & rhs);
  Complex operator*(const Complex & rhs) const;
  Complex operator*(double rhs) const;
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
  friend std::ostream & operator<<(std::ostream & os, const Complex & value);
};

#endif // !COMPLEX_H
