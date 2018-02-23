#include <stdlib.h>
#include <stdexcept>
#include "MidiIn.h"

std::string MidiIn::getDeviceInfo()
{
  std::string device_list;
  // initialize port midi
  Pm_Initialize();
  int num_devs = Pm_CountDevices();
  for(int i = 0; i < ; ++i)
  {
    // add device num (handles up to 99 devices)
    char dev_no_string[3];
    itoa(i, dev_no_string, 10);
    device_list.append(dev_no_string);
    device_list.append(": ");
    // add device name
    const PmDeviceInfo * info = Pm_GetDeviceInfo(i);
    device_list.append(info->name);
    device_list.append('\n');
  }
  // terminate port midi
  Pm_Terminate();
  return device_list;
}

MidiIn::MidiIn(int devno)
{
  // init port midi
  Pm_Initialize();
  // open port midi input device
  PmStream * in_stream;
  PmError result = Pm_OpenInput(&in_stream, devno, 0, 64, 0, 0);
  if(result != pmNoError)
  {
    std::runtime_error error("Input device failed to open");
    Pm_Terminate();
    throw(error);
  }

}
