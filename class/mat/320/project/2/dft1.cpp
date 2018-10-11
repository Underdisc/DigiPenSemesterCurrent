#include <cmath>
#include <fstream>
#include <iostream>
#include <ostream>
#include <stdlib.h>
#include <vector>

// Utility functions ///////////////////////////////////////////////////////////

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

// Complex /////////////////////////////////////////////////////////////////////

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
    Complex operator/(double rhs) const;
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

// Complex definitions -------------------------------------------------------//

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
    unsigned split_index = 0;
    unsigned end_index = 0;
    unsigned char complete = 0;
    std::string stripped_string = StripWhitespace(string_value);
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

// DFT & InverseDFT ////////////////////////////////////////////////////////////

#define TAU 6.28318530717958647692

void DFT(std::vector<Complex> * v_output, 
         const std::vector<Complex> & v_input, 
         unsigned num_samples)
{
    for(unsigned k = 0; k < num_samples; ++k)
    
    {
        Complex output(0.0, 0.0);
        for(unsigned n = 0; n < num_samples; ++n)
        {
            Complex addend;
            Complex unity_root;
            unity_root.Polar(1.0, (-TAU * k * n) / num_samples);
            addend = v_input[n] * unity_root; 
            output += addend;
        }
        v_output->push_back(output);
    }
}

void InverseDFT(std::vector<Complex> * v_output, 
         const std::vector<Complex> & v_input, 
         unsigned num_samples)
{

    for(unsigned k = 0; k < num_samples; ++k)
    
    {
        Complex output(0.0, 0.0);
        for(unsigned n = 0; n < num_samples; ++n)
        {
            Complex addend;
            Complex unity_root;
            unity_root.Polar(1.0, (TAU * k * n) / num_samples);
            addend = v_input[n] * unity_root; 
            output += addend;
        }
        v_output->push_back(output / num_samples);
    }
}

// Main ////////////////////////////////////////////////////////////////////////


int main(int argc, char ** argv)
{
    if(argc < 3)
    {
        std::cout << "Error: Incorrect number of arguments.\n"
                     "dft.exe N filename"
                  << std::endl;
        return 0;
    }

    unsigned samples = (unsigned)atoi(argv[1]);
    std::string filename(argv[2]);

    std::vector<Complex> v_input;
    Complex::FileToVComplex(&v_input, filename);

    if(samples > v_input.size())
    {
        std::cout << "Error: The N value provided is greater than the number "
                     "of complex numbers in the given file."
                  << std::endl;
        return 0;
    }

    std::vector<Complex> v_output;
    DFT(&v_output, v_input, samples);

    for(const Complex & c: v_output)
    {
        std::cout << c << std::endl;
    }
}
