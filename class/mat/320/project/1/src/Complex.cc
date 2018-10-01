#include <cmath>

#include "Complex.h"

Complex::Complex() : m_Real(0), m_Imaginary(0)
{}

Complex::Complex(double real, double imaginary) :
  m_Real(real), m_Imaginary(imaginary)
{}

Complex::Complex(const Complex & other) :
  m_Real(other.m_Real), m_Imaginary(other.m_Imaginary)
{}

void Complex::Polar(double magnitude, double angle)
{
    m_Real = magnitude * std::cos(angle);
    m_Imaginary = magnitude * std::sin(angle);
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

std::ostream & operator<<(std::ostream & os, const Complex & value)
{
    os << '(' << value.m_Real << ')';
    if(value.m_Imaginary >= 0.0f)
    {
        os << " + ";
    }
    else
    {
        os << " - ";
    }
    double imaginary_positive = std::abs(value.m_Imaginary);
    os << '(' << imaginary_positive << ')' << 'i';
    return os;
}
