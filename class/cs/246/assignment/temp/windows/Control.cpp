// Control.cpp
// -- Win32 GDI implementation
// jsh 9/18

#include <algorithm>
#include <sstream>
#include "Control.h"
using namespace std;


Control::Control(unsigned n, const char *title)
    : controls(n),
      name(title) {
  event = CreateEvent(0,TRUE,FALSE,0);
  thread = CreateThread(0,0,ThrdProc,this,0,0);
  WaitForSingleObject(event,INFINITE);
}


Control::~Control(void) {
  PostMessage(window,WM_DESTROY,0,0);
  WaitForSingleObject(thread,INFINITE);
  CloseHandle(thread);
}


LRESULT CALLBACK Control::WinProc(HWND win, UINT msg, WPARAM wp, LPARAM lp) {

  switch (msg) {
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_HSCROLL:
      {
        unsigned i = GetWindowLong((HWND)lp,GWL_ID);
        Control *control = reinterpret_cast<Control*>
                            (GetWindowLongPtr(win,GWLP_USERDATA));
        switch (LOWORD(wp)) {
          case SB_THUMBTRACK:
            control->setValue(i,short(HIWORD(wp)));
            break;
          case SB_LINELEFT:
            {
              SCROLLINFO info;
              info.cbSize = sizeof(SCROLLINFO);
              info.fMask = SIF_POS|SIF_RANGE;
              GetScrollInfo(control->controls[i].scroll_handle,SB_CTL,&info);
              int value = max(info.nMin,info.nPos-1);
              control->setValue(i,value);
              break;
            }
          case SB_LINERIGHT:
            {
              SCROLLINFO info;
              info.cbSize = sizeof(SCROLLINFO);
              info.fMask = SIF_POS|SIF_RANGE;
              GetScrollInfo(control->controls[i].scroll_handle,SB_CTL,&info);
              int value = min(info.nMax,info.nPos+1);
              control->setValue(i,value);
              break;
            }
        }
        return 0;
      }
    default:
      return DefWindowProc(win,msg,wp,lp);
  }
}


DWORD WINAPI Control::ThrdProc(LPVOID vp) {
  Control *cntrl = reinterpret_cast<Control*>(vp);

  WNDCLASS wc;
  ZeroMemory(&wc,sizeof(wc));
  wc.lpfnWndProc = WinProc;
  wc.hInstance = GetModuleHandle(0);
  wc.hCursor = LoadCursor(0,IDC_ARROW);
  wc.hbrBackground = COLOR_SCROLLBAR;
  wc.lpszClassName = cntrl->name;
  RegisterClass(&wc);

  int min_height = GetSystemMetrics(SM_CYSIZE);
  int width = GetSystemMetrics(SM_CXSCREEN)/4,
      scr_width = int(0.9*width),
      scr_height = max(min_height,width/20),
      scr_offset_x = (width - scr_width)/2,
      scr_offset_y = int(1.5*scr_height),
      dheight = 3 * scr_height,
      height = cntrl->controls.size() * dheight;
  RECT rect = {0,0, width,height};
  AdjustWindowRect(&rect,WS_POPUP|WS_CAPTION,FALSE);

  cntrl->window = CreateWindow(cntrl->name,cntrl->name,WS_POPUP|WS_CAPTION,
                               CW_USEDEFAULT,CW_USEDEFAULT,
                               rect.right-rect.left,rect.bottom-rect.top,
                               0,0,wc.hInstance,0);
  SetWindowLongPtr(cntrl->window,GWLP_USERDATA,(LONG_PTR)cntrl);
  for (unsigned i=0; i < cntrl->controls.size(); ++i) {
    CreateWindow(TEXT("static"),0,WS_CHILD|WS_VISIBLE|WS_BORDER,
                 0,i*dheight,width,dheight,cntrl->window,0,wc.hInstance,0);
    stringstream s;
    s << "Control #" << (i+1);
    cntrl->controls[i].label_handle
      = CreateWindow(TEXT("static"),s.str().c_str(),WS_CHILD|WS_VISIBLE,
                     scr_offset_x/2,i*dheight+scr_height/2,scr_width,scr_height,
                     cntrl->window,0,wc.hInstance,0);
    cntrl->controls[i].scroll_handle
      = CreateWindow(TEXT("scrollbar"),0,WS_CHILD|WS_VISIBLE|SBS_HORZ,
                     scr_offset_x,i*dheight+scr_offset_y,scr_width,scr_height,
                     cntrl->window,(HMENU)i,wc.hInstance,0);
    SCROLLINFO info;
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_RANGE | SIF_POS;
    info.nMin = 0;
    info.nMax = 100;
    info.nPos = 0;
    SetScrollInfo(cntrl->controls[i].scroll_handle,SB_CTL,&info,FALSE);
  }
  //ShowWindow(cntrl->window,SW_SHOW);
  SetEvent(cntrl->event);

  MSG msg;
  while (GetMessage(&msg,cntrl->window,0,0)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  UnregisterClass(cntrl->name,0);
  return 0;
}


void Control::setRange(unsigned i, int min, int max) {
    SCROLLINFO info;
    info.cbSize = sizeof(SCROLLINFO);
    info.fMask = SIF_RANGE;
    info.nMin = min;
    info.nMax = max;
    SetScrollInfo(controls[i].scroll_handle,SB_CTL,&info,FALSE);
}

void Control::setLabel(unsigned i, const char *label) {
  SetWindowText(controls[i].label_handle,label);
}


void Control::setValue(unsigned id, int value) {
  SCROLLINFO info;
  info.cbSize = sizeof(SCROLLINFO);
  info.fMask = SIF_POS;
  info.nPos = value;
  SetScrollInfo(controls[id].scroll_handle,SB_CTL,&info,TRUE);
  valueChanged(id,value);
}


void Control::show(bool visible) {
  ShowWindow(window,visible?SW_SHOW:SW_HIDE);
}


void Control::setTitle(const char *title) {
  SetWindowText(window,title);
}

