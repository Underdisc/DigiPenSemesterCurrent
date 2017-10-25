/* All content(c) 2017 - 2018 DigiPen(USA) Corporation, all rights reserved. */
/*****************************************************************************/
/*!
\file Framer.cpp
\author Connor Deakin
\par E-mail: connor.deakin\@digipen.edu
\par Project: Graphics
\date 2017/10/23
\brief
  Implementation of the Framer utility class.
*/
/*****************************************************************************/
#include <chrono>
#include <thread>

#include "Time.h"
#include "Framer.h"

// macros
#define MILLISINS 1000.0f // milliseconds in a second

// static initialization
bool Framer::_locked = false;
float Framer::_targetFrameTime = 0.0f;
float Framer::_startTime = 0.0f;
float Framer::_timeSinceLastFPSCalculation = 0.0f;
float Framer::_waitTimeForFPSCalculation = 1.0f;
float Framer::_averageFPS = 0.0f;
std::vector<float> Framer::_frameTimes;

/*****************************************************************************/
/*!
\brief
  Call this at the start of a frame to save the current time in the Framer.
*/
/*****************************************************************************/
void Framer::Start()
{
  _startTime = Time::TotalTimeExact();
}

/*****************************************************************************/
/*!
\brief
  Call this at the end of a frame to lock the frame if locking is enabled and
  save time values associated with the frame.
*/
/*****************************************************************************/
void Framer::End()
{
  float end_time = Time::TotalTimeExact();
  float time_passed = end_time - _startTime;
  if(_locked && time_passed < _targetFrameTime){
    // time to wait in seconds 
    float s_ttw = _targetFrameTime - time_passed;
    float ms_ttw = s_ttw * MILLISINS;
    // waiting
    std::this_thread::sleep_for(std::chrono::milliseconds((int)ms_ttw));
    //updating time passed
    time_passed += s_ttw;
  }
  // save time pased
  _frameTimes.push_back(time_passed);
  _timeSinceLastFPSCalculation += time_passed;
  // calculate average fps if necessary
  if(_timeSinceLastFPSCalculation >= _waitTimeForFPSCalculation)
    CalculateAverageFPS();
}

/*****************************************************************************/
/*!
\brief
  Unlocks the FPS. Stopes the Framer from locking at the end of a frame.
*/
/*****************************************************************************/
void Framer::Unlock()
{
  _locked = false;
}

/*****************************************************************************/
/*!
\brief
  Enables frame locking. Once called, the Framer will lock frame rate at 
  the desired fps at the end of each frame.

\param fps
  The fps the Framer will try to maintain.
*/
/*****************************************************************************/
void Framer::Lock(int fps)
{
  _locked = true;
  _targetFrameTime = 1.0f / (float)fps;
}

/*****************************************************************************/
/*!
\brief
  Returns the average FPS over the last duration specified by 
  _waitTimeForFPSCalculation.

\return The average FPS.
*/
/*****************************************************************************/
float Framer::AverageFPS()
{
  return _averageFPS;
}

/*****************************************************************************/
/*!
\brief
  Calculates the average fps with the current values stored in _frameTimes.
*/
/*****************************************************************************/
void Framer::CalculateAverageFPS()
{
  // finding total time
  float total_time = 0.0f;
  for(const float & frame_time : _frameTimes)
    total_time += frame_time;
  // finding averages
  float average_frame_time = total_time / (float)_frameTimes.size();
  _averageFPS = 1.0f / average_frame_time;
  // resetting values
  _frameTimes.clear();
  _timeSinceLastFPSCalculation = 0.0f;
}
