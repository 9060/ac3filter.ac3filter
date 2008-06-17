/*
  Spectrum controls
*/

#ifndef CONTROL_SPECTRUM_H
#define CONTROL_SPECTRUM_H

#include "../guids.h"
#include "../controls.h"

class ControlSpectrum : public Controller
{
protected:
  IAudioProcessor *proc;

  HWND hctrl;
  RECT client_rect;
  int width, height;

  HDC     mem_dc;
  HBITMAP mem_bitmap;
  HBITMAP old_bitmap;

  HBRUSH bkg_brush;
  HPEN   signal_pen;
  HPEN   grid_pen;

  size_t spectrum_length;
  sample_t *spectrum;

public:
  ControlSpectrum(HWND dlg, IAudioProcessor *proc);
  ~ControlSpectrum();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();
  void paint();

  virtual cmd_result command(int control, int message);
};

#endif
