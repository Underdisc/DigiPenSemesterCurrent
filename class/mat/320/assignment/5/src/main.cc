#include <iostream>
#include <math.h>
#include <vector>

#include "Complex.h"

#define TAU 6.28318530717958647692

class Filter
{
public:
    Filter();
    double MagnitudeResponse(double omega);
    void AddZero(const Complex & z);
    void AddPole(const Complex & p);
    std::vector<Complex> _zeros;
    std::vector<Complex> _poles;
};

Filter::Filter() : _zeros(), _poles()
{}

double Filter::MagnitudeResponse(double omega)
{
    Complex z;
    z.Polar(1.0, omega);

    // Calculate the numerator for the magnitude response.
    double numer = 1.0;
    for(const Complex & c : _zeros)
    {
        Complex difference = z - c;
        numer *= difference.Magnitude();
    }

    // Calculate the denominator for the magnitude response.
    double denom = 1.0;
    for(const Complex & c : _poles)
    {
        Complex difference = z - c;
        denom *= difference.Magnitude();
    }
    return numer / denom;
}

void Filter::AddZero(const Complex & z)
{
    _zeros.push_back(z);
}

void Filter::AddPole(const Complex & p)
{
    _poles.push_back(p);
}

int main(void)
{
    // The zeros of the filter.
    Complex za(0.81, 0.59);
    Complex zb(0.48, 0.88);
    Complex zc(-0.66, 0.75);
    Complex zd(-0.89, 0.46);

    // The poles of the filter.
    Complex pa(0.21, 0.91);
    Complex pb(0.04, 0.80);
    Complex pc(-0.25, 0.76);
    Complex pd(-0.44, 0.83);

    // Add all of the zeros and the poles to the filter.
    Filter main_filter;
    main_filter.AddZero(za);
    main_filter.AddZero(za.Conjugate());
    main_filter.AddZero(zb);
    main_filter.AddZero(zb.Conjugate());
    main_filter.AddZero(zc);
    main_filter.AddZero(zc.Conjugate());
    main_filter.AddZero(zd);
    main_filter.AddZero(zd.Conjugate());

    main_filter.AddPole(pa);
    main_filter.AddPole(pa.Conjugate());
    main_filter.AddPole(pb);
    main_filter.AddPole(pb.Conjugate());
    main_filter.AddPole(pc);
    main_filter.AddPole(pc.Conjugate());
    main_filter.AddPole(pd);
    main_filter.AddPole(pd.Conjugate());

    // Calculate the magnitude response of the filter for the selected values
    // of fractions of the sampling frequency..
    for(double f = 0.18; f < 0.37; f += 0.01)
    {
        double omega = f * TAU;
        double mr = main_filter.MagnitudeResponse(omega);
        mr = mr / 40.0;
        double db = 20.0 * log10(mr);
        std::cout << "|H(" << f << " * TAU)| = " << mr << std::endl;
    }

    return 0;
}
