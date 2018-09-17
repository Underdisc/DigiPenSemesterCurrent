// Control.cpp
// -- SDL2/OpenGL (immediate mode) implementation
// jsh 9/18

#include <algorithm>
#include <cmath>
#include <cctype>
#include "Control.h"
using namespace std;


float Control::SliderData::aspect = 1.0f;


/////////////////////////////////////////////////////////////////
// ASCII to 5x8 dot matrix code
/////////////////////////////////////////////////////////////////
struct DotMatrixCode {
  DotMatrixCode(void);
  ~DotMatrixCode(void);
  const unsigned char *getCode(char c);
  unsigned char *codes;
};


const unsigned char table_keys[26+10+6]
  = { 'A','B','C','D','E','F','G','H','I','J','K','L','M',
      'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
      '0','1','2','3','4','5','6','7','8','9',
      ' ',',',':','.','-', '%' };


const unsigned char table_vals[5*(26+10+6)]
  = { 0x7e,0x88,0x88,0x88,0x7e, // A
      0xfe,0x92,0x92,0x92,0x6c, // B
      0x7c,0x82,0x82,0x82,0x44, // C
      0xfe,0x82,0x82,0x82,0x7c, // D
      0xfe,0x92,0x92,0x92,0x82, // E
      0xfe,0x90,0x90,0x90,0x80, // F
      0x7c,0x82,0x82,0x8a,0x4c, // G
      0xfe,0x10,0x10,0x10,0xfe, // H
      0x00,0x82,0xfe,0x82,0x00, // I
      0x0c,0x02,0x02,0x02,0xfc, // J
      0xfe,0x10,0x28,0x44,0x82, // K
      0xfe,0x02,0x02,0x02,0x02, // L
      0xfe,0x40,0x20,0x40,0xfe, // M
      0xfe,0x20,0x10,0x08,0xfe, // N
      0x7c,0x82,0x82,0x82,0x7c, // O
      0xfe,0x88,0x88,0x88,0x70, // P
      0x7c,0x82,0x8a,0x86,0x7e, // Q
      0xfe,0x88,0x88,0x88,0x76, // R
      0x64,0x92,0x92,0x92,0x4c, // S
      0x80,0x80,0xfe,0x80,0x80, // T
      0xfc,0x02,0x02,0x02,0xfc, // U
      0xf8,0x04,0x02,0x04,0xf8, // V
      0xfe,0x04,0x08,0x04,0xfe, // W
      0xc6,0x28,0x10,0x28,0xc6, // X
      0xe0,0x10,0x0e,0x10,0xe0, // Y
      0x86,0x8a,0x92,0xa2,0xc2, // Z
      0xfe,0x82,0x82,0x82,0xfe, // 0
      0x00,0x00,0x40,0xfe,0x00, // 1
      0x9e,0x92,0x92,0x92,0xf2, // 2
      0x92,0x92,0x92,0x92,0xfe, // 3
      0xf0,0x10,0x10,0x10,0xfe, // 4
      0xf2,0x92,0x92,0x92,0x9e, // 5
      0xfe,0x92,0x92,0x92,0x9e, // 6
      0x80,0x80,0x8e,0x90,0xe0, // 7
      0xfe,0x92,0x92,0x92,0xfe, // 8
      0xf2,0x92,0x92,0x92,0xfe, // 9
      0x00,0x00,0x00,0x00,0x00, // ' '
      0x00,0x00,0x03,0x00,0x00, // ','
      0x00,0x00,0x44,0x00,0x00, // ':'
      0x00,0x00,0x02,0x00,0x00, // '.'
      0x10,0x10,0x10,0x10,0x10, // '-'
      0xc4,0xc8,0x10,0x26,0x46  // '%'
    };


DotMatrixCode::DotMatrixCode(void) {
  codes = new unsigned char[5*128];
  for (int i=0; i < 128; ++i) {
    codes[5*i+0] = 0xff;
    codes[5*i+1] = 0x81;
    codes[5*i+2] = 0x81;
    codes[5*i+3] = 0x81;
    codes[5*i+4] = 0xff;
  }
  for (int i=0; i < 42; ++i)
    for (int j=0; j < 5; ++j)
      codes[5*table_keys[i]+j] = table_vals[5*i+j];
}


DotMatrixCode::~DotMatrixCode(void) {
  delete[] codes;
}


const unsigned char *DotMatrixCode::getCode(char c) {
  return codes + 5*toupper(c);
}


DotMatrixCode dot_map;


/////////////////////////////////////////////////////////////////
// Control implementation
/////////////////////////////////////////////////////////////////
Control::Control(unsigned n, const char *name)
    : slider_data(n),
      slider_selected(-1) {
  for (unsigned i=0; i < n; ++i)
    slider_data[i].value = 0.5f;
//cout << "constructor: starting thread" << endl;
  semaphore = SDL_CreateSemaphore(0);
  thread = SDL_CreateThread(ThreadFunc,"Control::ThreadFunc",this);
  SDL_SemWait(semaphore);
  setTitle(name);
//cout << "constructor: finished" << endl;
}


Control::~Control(void) {
//cout << "destructor: waiting for thread" << endl;
  SDL_Event e;
  e.type = SDL_QUIT;
  SDL_PushEvent(&e);
  SDL_WaitThread(thread,0);
  SDL_DestroySemaphore(semaphore);
//cout << "destructor: finished" << endl;
}


int Control::ThreadFunc(void *vp) {
//cout << "thread start" << endl;
  Control *cntrl = reinterpret_cast<Control*>(vp);

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Rect rect;
  SDL_GetDisplayBounds(0,&rect);
  int width = int(0.5f*rect.w),
      height = (cntrl->slider_data.size())*int(0.1f*rect.h);

  cntrl->slider_data[0].aspect = float(width)/float(height);
  float dy = 2.0f/cntrl->slider_data.size(),
        pad = min(0.03f*dy,0.03f*width);
  for (unsigned i=0; i < cntrl->slider_data.size(); ++i) {
    cntrl->slider_data[i].x = -1 + pad;
    cntrl->slider_data[i].y = 1 - (i+1)*dy + pad;
    cntrl->slider_data[i].w = 2 - 2*pad;
    cntrl->slider_data[i].h = dy - 2*pad;
    cntrl->slider_data[i].value = 400;
    cntrl->slider_data[i].min_value = 0;
    cntrl->slider_data[i].max_value = 1000;
  }

  cntrl->window = SDL_CreateWindow("Control Class 0.2",SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED,width,height,
                                   SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
                                   |SDL_WINDOW_HIDDEN);
  SDL_GLContext context = SDL_GL_CreateContext(cntrl->window);

  SDL_SemPost(cntrl->semaphore);
//cout << "thread initialized" << endl;

  bool done = false;
  while (!done) {
    SDL_Event event;
    SDL_WaitEvent(&event);
    switch (event.type) {
      case SDL_QUIT:
        done = true;
        break;
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
          case SDL_WINDOWEVENT_EXPOSED:
            cntrl->Draw();
            break;
          case SDL_WINDOWEVENT_SIZE_CHANGED:
            width = event.window.data1;
            height = event.window.data2;
            Control::SliderData::aspect = float(width)/float(height);
            glViewport(0,0,width,height);
            break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (event.button.button == SDL_BUTTON_LEFT) {
          float x = 2*float(event.button.x)/float(width) - 1,
                y = 1-2*float(event.button.y)/float(height);
          int i = 0;
          while (i < cntrl->slider_data.size()
                 && !cntrl->InsideSliderThumb(x,y,cntrl->slider_data[i]))
            ++i;
          if (i < cntrl->slider_data.size())
            cntrl->slider_selected = i;
        }
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.button == SDL_BUTTON_LEFT)
          cntrl->slider_selected = -1;
        break;
      case SDL_MOUSEMOTION:
        if (cntrl->slider_selected >= 0) {
          SliderData &s = cntrl->slider_data[cntrl->slider_selected];
          float x = 2*float(event.motion.x)/float(width) - 1,
                dx = 0.5f*s.h/s.aspect,
                t = 0.5f*(x + 1);
          int value = floor((1-t)*s.min_value + t*s.max_value + 0.5f);
          value = max(s.min_value,min(s.max_value,value));
          if (s.value != value) {
            s.value = value;
            cntrl->valueChanged(cntrl->slider_selected,value);
            cntrl->DrawSlider(s);
            SDL_GL_SwapWindow(cntrl->window);
          }
        }
        break;
    }
  }

  SDL_DestroyWindow(cntrl->window);
  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}


void Control::Draw(void) {

  glClearColor(0.9f,0.95f,0.9f,1);
  glClear(GL_COLOR_BUFFER_BIT);

  for (int i=0; i < slider_data.size(); ++i)
    DrawSlider(slider_data[i]);

  SDL_GL_SwapWindow(window);
}


void Control::DrawSlider(const SliderData& s) {
  float h2 = 0.5f*s.h;

  // background
  glColor4f(0.9f,0.9f,0.9f,1);
  glBegin(GL_TRIANGLE_FAN);
    glVertex2f(s.x,s.y);
    glVertex2f(s.x+s.w,s.y);
    glVertex2f(s.x+s.w,s.y+s.h);
    glVertex2f(s.x,s.y+s.h);
  glEnd();

  // thumb track
  glColor4f(0.5f,0.5f,1,1);
  glBegin(GL_TRIANGLE_FAN);
    glVertex2f(s.x,s.y);
    glVertex2f(s.x+s.w,s.y);
    glVertex2f(s.x+s.w,s.y+h2);
    glVertex2f(s.x,s.y+h2);
  glEnd();

  // thumb
  glColor4f(1,0.5f,0.5f,1);
  float yc = s.y + 0.5f*h2,
        dx = h2/s.aspect,
        t = float(s.value-s.min_value)/float(s.max_value-s.min_value),
        xc = s.x + 0.5f*dx + t*(s.w - dx);
  glBegin(GL_TRIANGLE_FAN);
    glVertex2f(xc-0.5f*dx,yc-0.5f*h2);
    glVertex2f(xc+0.5f*dx,yc-0.5f*h2);
    glVertex2f(xc+0.5f*dx,yc+0.5f*h2);
    glVertex2f(xc-0.5f*dx,yc+0.5f*h2);
  glEnd();
  glColor4f(0,0,0,1);
  glBegin(GL_LINE_LOOP);
    glVertex2f(xc-0.5f*dx,yc-0.5f*h2);
    glVertex2f(xc+0.5f*dx,yc-0.5f*h2);
    glVertex2f(xc+0.5f*dx,yc+0.5f*h2);
    glVertex2f(xc-0.5f*dx,yc+0.5f*h2);
  glEnd();

  // label
  glColor4f(0,0,0,1);
  float dy = s.h/32.0f;
  dx = dy/s.aspect;
  for (unsigned n=0; n < s.label.size(); ++n) {
    const unsigned char *code = dot_map.getCode(s.label[n]);
    for (int i=0; i < 5; ++i) {
      float x = s.x + dx + 6*n*dx + i*dx;
      for (int j=0; j < 8; ++j) {
        float y = s.y + 0.125f*5*s.h + j*dy;
        unsigned char mask = 1 << j;
        if ((code[i] & mask) != 0) {
          glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x,y);
            glVertex2f(x+dx,y);
            glVertex2f(x+dx,y+dy);
            glVertex2f(x,y+dy);
          glEnd();
        }
      }
    }
  }

  // border
  glColor4f(0,0,0,1);
  glBegin(GL_LINE_LOOP);
    glVertex2f(s.x,s.y);
    glVertex2f(s.x+s.w,s.y);
    glVertex2f(s.x+s.w,s.y+s.h);
    glVertex2f(s.x,s.y+s.h);
  glEnd();
}


bool Control::InsideSliderThumb(float x, float y, const SliderData& s) {
  float t = float(s.value-s.min_value)/float(s.max_value-s.min_value),
        dx = 0.5f*s.h/s.aspect,
        xc = s.x + 0.5f*dx + t*(s.w - dx),
        yc = s.y + 0.25f*s.h;
  return abs(x-xc) <= 0.5f*dx && abs(y-yc) <= 0.25f*s.h;
}


void Control::setRange(unsigned index, int min, int max) {
  slider_data[index].min_value = min;
  slider_data[index].max_value = max;
}


void Control::setLabel(unsigned index, const char *label) {
  slider_data[index].label = label;
}


void Control::setValue(unsigned index, int value) {
  slider_data[index].value = value;
  valueChanged(index,value);
}


void Control::show(bool visible) {
  if (visible)
    SDL_ShowWindow(window);
  else
    SDL_HideWindow(window);
}


void Control::setTitle(const char *title) {
  SDL_SetWindowTitle(window,title);
}

