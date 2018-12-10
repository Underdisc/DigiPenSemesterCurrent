////////////////////////////////////////////////////////////////////////////////
// File:        Equalizer.h
// Class:       CS246
// Asssignment: 5
// Author:      Connor Deakin
// Email:       connor.deakin@digipen.edu
// Date:        2018-12-06
////////////////////////////////////////////////////////////////////////////////

#include <vector>

#include "BandPass.h"

class Equalizer : public Filter 
{
public:
    Equalizer(int n=1, float R=44100);
    ~Equalizer(void);
    int bandCount(void) const;
    void setGain(int n, float g);
    float operator()(float x);
private:
    struct SubFilter
    {
        BandPass _filter;
        float _gain;
    };
    // The BandPass filters and their corresponding gains.
    std::vector<SubFilter> _filters;
};
