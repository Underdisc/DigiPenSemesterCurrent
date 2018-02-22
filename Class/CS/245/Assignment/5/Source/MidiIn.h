// MidiIn
// -- MIDI input framework
// cs245 2/18

#ifndef CS245_MIDIIN_H
#define CS245_MIDIIN_H

#include <string>
#include "pthread.h"
#include "portmidi.h"


class MidiIn {
  public:
    static std::string getDeviceInfo(void);
    MidiIn(int devno);
    void start();
    void stop();
    virtual ~MidiIn(void);
    virtual void onNoteOn(int channel, int note, int velocity) { }
    virtual void onNoteOff(int channel, int note) { }
    virtual void onPitchWheelChange(int channel, float value) { }
    virtual void onVolumeChange(int channel, int level) { }
    virtual void onModulationWheelChange(int channel, int value) { }
    virtual void onControlChange(int channel, int number, int value) { }
  private:
    PmStream *input_stream;
    bool process_events,
         thread_running;
    pthread_t event_thread;
    static void* eventLoop(void *vp);
};


#endif
