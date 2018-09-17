// ControlTest.cpp
// -- Simple example of using controls
// cs246 9/18
//
// To compile from the Visual Studio Command Prompt:
//   cl /EHsc ControlTest.cpp Control.cpp user32.lib
// Or,
//   cl /EHsc ControlTest.cpp Control.lib user32.lib

#include <iostream>
#include <sstream>
#include "Control.h"
using namespace std;


class AudioControl : public Control {
  public:
    AudioControl(void);
    void valueChanged(unsigned id, int value);
};

const char *labels[] = { "Fred", "Barney", "Wilma" };


AudioControl::AudioControl(void) : Control(3) {
  setRange(0,0,100);
  setLabel(0,labels[0]);
  setValue(0,10);

  setRange(1,-5,5);
  setLabel(1,labels[1]);
  setValue(1,3);

  setRange(2,0,1000);
  setLabel(2,labels[2]);
  setValue(2,723);
}


void AudioControl::valueChanged(unsigned num, int value) {
  cout << num << " : " << value << endl;
  stringstream s;
  s << labels[num] << " : " << value;
  setLabel(num,s.str().c_str());
}


int main(int argc, char *argv[]) {

  AudioControl *cntrl = new AudioControl();
  cntrl->show(true);

  cin.get();
  cntrl->show(false);

  delete cntrl;
  return 0;
}

