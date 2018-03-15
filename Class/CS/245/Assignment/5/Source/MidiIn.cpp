/*****************************************************************************/
/*!
\file MidiIn.cpp
\author Connor Deakin
\par E-mail: connortdeakin\@gmail.com
\par Assignment: 5
\par Course: CS 245
\par Term: Spring 2018
\date 05/03/2018
\brief
  Contains the implementation for the MidiIn class. MidiIn class is used
  for processsing midi events that are recieved from the given device number.
*/
/*****************************************************************************/

#include <stdlib.h>
#include <stdexcept>
#include <utility>
#include "MidiIn.h"

typedef unsigned char uchar;

std::string MidiIn::getDeviceInfo()
{
  std::string device_list;
  // initialize port midi
  Pm_Initialize();
  int num_devs = Pm_CountDevices();
  for(int i = 0; i < num_devs; ++i)
  {
    // add device num (handles up to 99 devices)
    char dev_no_string[3];
    itoa(i, dev_no_string, 10);
    device_list.append(dev_no_string);
    device_list.append(": ");
    // add device name
    const PmDeviceInfo * info = Pm_GetDeviceInfo(i);
    device_list.append(info->name);
    device_list.append("\n");
  }
  // terminate port midi
  Pm_Terminate();
  return device_list;
}

MidiIn::MidiIn(int devno) : process_events(false), thread_running(true)
{
  // init port midi
  Pm_Initialize();
  // open port midi input device
  PmError result = Pm_OpenInput(&input_stream, devno, 0, 64, 0, 0);
  if(result != pmNoError)
  {
    std::runtime_error error("Input device failed to open");
    Pm_Terminate();
    throw(error);
  }
  // create thread for midi events
  pthread_create(&event_thread, 0, eventLoop, this);
}

MidiIn::~MidiIn()
{
  // shutdown event thread
  process_events = false;
  thread_running = false;
  pthread_join(event_thread, 0);
  // close input stream
  Pm_Close(input_stream);
  Pm_Terminate();
}

void MidiIn::start()
{
  process_events = true;
}

void MidiIn::stop()
{
  process_events = false;
}

// format of a midi message
struct MidiMessage
{
  union
  {
    long message;
    unsigned char byte[4];
  };
};

// event types
enum EventType
{
  NOTEOFF = 8,
  NOTEON = 9,
  CONTROL = 11,
  PITCHBEND = 14,
};

// control types
enum ControlType
{
  MODULATION = 1,
  VOLUME = 7,
};

// splits byte in half into two 4 bit values and stores the values into two
// new bytes
std::pair<uchar, uchar> splitByte(uchar byte)
{
  std::pair<uchar, uchar> split_byte;
  split_byte.first = byte >> 4;
  split_byte.second = byte & 0x0f;
  return split_byte;
}

void * MidiIn::eventLoop(void * vp)
{
  MidiIn & midi_in = *(MidiIn *)vp;
  while(midi_in.thread_running)
  {
    // continue if we are not processing events
    if(!midi_in.process_events)
      continue;

    // poll for event - continue if there is no event
    if(!Pm_Poll(midi_in.input_stream))
      continue;
    // read in event
    PmEvent event;
    Pm_Read(midi_in.input_stream, &event, 1);
    MidiMessage msg;
    msg.message = event.message;
    // find event type and channel
    std::pair<uchar, uchar> split_byte = splitByte(msg.byte[0]);
    int event_type = (int)split_byte.first;
    int channel = (int)split_byte.second;
    // call according to event type
    if(event_type == NOTEOFF)
    {
      // NOTEOFF event
      int note = (int)msg.byte[1];
      midi_in.onNoteOff(channel, note);
    }
    else if(event_type == NOTEON)
    {
      // NOTEON event
      int note = (int)msg.byte[1];
      int velocity = (int)msg.byte[2];
      if(velocity == 0)
        midi_in.onNoteOff(channel, note);
      else
        midi_in.onNoteOn(channel, note, velocity);
    }
    else if(event_type == CONTROL)
    {
      // CONTROL event
      int control_number = (int)msg.byte[1];
      int value = (int)msg.byte[2];
      if (control_number == MODULATION)
      {
        midi_in.onModulationWheelChange(channel, value);
      }
      else if(control_number == VOLUME)
      {
        midi_in.onVolumeChange(channel, value);
      }
      midi_in.onControlChange(channel, control_number, value);
    }
    else if(event_type == PITCHBEND)
    {
      // PITCHBEND event
      int little = (int)msg.byte[1];
      int big = (int)msg.byte[2];
      int int_value = (big * 128 + little) - 8192;
      float value = (float)(int_value * 2) / 16383.0f;
      midi_in.onPitchWheelChange(channel, value);
    }
  }
  return nullptr;
}
