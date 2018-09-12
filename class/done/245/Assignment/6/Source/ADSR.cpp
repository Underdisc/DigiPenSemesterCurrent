/*****************************************************************************/
/*!
\file ADSR.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Assignment: 6
\par Course: CS 245
\par Term: Spring 2018
\date 25/03/2018
\brief
  Contains the implementation of the ADSR envelope.
*/
/*****************************************************************************/

#include "ADSR.h"
#include <cmath>

#define E 2.718281828459045235360f
#define EFFECTIVE_ZERO_16BIT 0.0000158489319246f

ADSR::ADSR(float a, float d, float s, float r, float R)
{
  // initializations for each phase
  // attack phase
  if(a == 0.0f)
  {
    envelope = 1.0f;
    mode = DECAY;
  }
  else
  {
    attack_increment = 1.0f / (R * a);
    envelope = 0.0f;
    mode = ATTACK;
  }
  // decay phase
  // log is equivalent to log base e
  if(d == 0.0f)
  {
    decay_factor = 0.0f;
  }
  else
  {
    float kd = std::log(EFFECTIVE_ZERO_16BIT) / -d;
    decay_factor = std::pow(E, -kd / R);
  }
  // sustain phase
  sustain_level = s;
  // release phase
  if(r == 0.0f)
  {
    release_factor = 0.0f;
  }
  else
  {
    float kr = std::log(EFFECTIVE_ZERO_16BIT) / -r;
    release_factor = std::pow(E, -kr / R);
  }
}

float ADSR::operator()(void)
{
  // update envelope value according to phase
  switch (mode)
  {
  // attack phase
  case ATTACK:
    envelope += attack_increment;
    if(envelope >= 1.0f)
    {
      envelope = 1.0f;
      mode = DECAY;
    }
    break;
  // decay phase
  case DECAY:
    envelope *= decay_factor;
    if(envelope <= sustain_level)
    {
      envelope = sustain_level;
      mode = SUSTAIN;
    }
    break;
  // no update needed for sustain phase
  // release phase
  case RELEASE:
    envelope *= release_factor;
    break;
  }
  // return updated envelope value
  return envelope;
}

void ADSR::sustainOff()
{
  mode = RELEASE;
}
