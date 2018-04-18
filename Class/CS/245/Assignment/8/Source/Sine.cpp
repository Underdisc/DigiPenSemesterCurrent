/*****************************************************************************/
/*!
\file Sine.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Assignment: 8
\par Course: CS 245
\par Term: Spring 2018
\date 26/03/2018
\brief
  Contains the implementation of the sine lookup table.
*/
/*****************************************************************************/

#include <stdexcept>
#include "Sine.h"

#include <iostream>

#define TAU 6.28318531
#define TAUF 6.28318531f

Sine::Sine(unsigned R)
{
  sine_table.resize(R);
  // calculate initial values
  // yn = beta * yn-1 - yn-2
  double yn_2 = 0.0; // yn-2
  double yn_1 = std::sin(TAU / (double)R); // yn-1
  double beta = 2.0 * std::cos(TAU / (double)R);
  // initialize first elements
  if(R >= 2)
  {
    sine_table[0] = 0.0f;
    sine_table[1] = (float)yn_1;
  }
  // throw error if less than two samples are requested
  // this would defeat the purpose of this class
  else
  {
    std::runtime_error error("R should be at least greater than 2");
    throw(error);
  }
  // compute remaining table values
  for(unsigned i = 2; i < R; ++i)
  {
    double yn = beta * yn_1 - yn_2;
    sine_table[i] = (float)yn;
    yn_2 = yn_1;
    yn_1 = yn;


    if(i < 30)
    {
      std::cout << yn << std::endl;
    }
  }
}

float Sine::operator()(float t)
{
  const size_t R = sine_table.size();
  // find fractional index into table
  float index = (t * (float)R) / TAUF;
  // seperate into integer and fractional part
  int int_part = (int)index;
  float fract_part = index - (float)int_part;
  // bring int_part into range [0, R)
  int_part = int_part % R;
  if(int_part < 0)
  {
    int_part += R;
    fract_part += 1.0f;
  }
  // calculate the value of sine using the lookup table
  float sample0 = sine_table[int_part];
  float sample1 = sine_table[(int_part + 1) % R];
  float final_sample = sample0 + fract_part * (sample1 - sample0);
  return final_sample;
}
