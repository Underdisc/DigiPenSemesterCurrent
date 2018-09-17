// Control.h
// -- interface for cross-platform audio controls?
// jsh 9/18 (SDL2/OpenGL version)

#ifndef JSH_CONTROL_H
#define JSH_CONTROL_H

#include <vector>
#include <string>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <GL/gl.h>


class Control {
  public:
    Control(unsigned n, const char *name="Control Class 0.2");
    virtual ~Control(void);
    void setRange(unsigned index, int min, int max);
    void setLabel(unsigned index, const char *label);
    void setValue(unsigned index, int value);
    void show(bool visible=true);
    void setTitle(const char *title);
    virtual void valueChanged(unsigned index, int value) {};
  private:
    SDL_Thread *thread;
    SDL_sem *semaphore;
    static int ThreadFunc(void *);
    void Draw(void);
    struct SliderData {
      float x, y, w, h;
      int value;
      static float aspect;
      int min_value, max_value;
      std::string label;
    };
    void DrawSlider(const SliderData& s);
    bool InsideSliderThumb(float x, float y, const SliderData& s);
    std::vector<SliderData> slider_data;
    int slider_selected;
    SDL_Window *window;
};


#endif

