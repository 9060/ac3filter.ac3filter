#ifndef AC3FILTER_DLG_H
#define AC3FILTER_DLG_H

#include <streams.h>
#include "controls.h"
#include "guids.h"


          
class AC3FilterDlg : public CBasePropertyPage, public AudioProcessorState
{
public:
  static CUnknown * WINAPI CreateMain  (LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateMixer (LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateGains (LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateSystem(LPUNKNOWN lpunk, HRESULT *phr);

  int flags;

  void reload_state();

private:
  IAC3Filter      *filter;
  IAudioProcessor *proc;
  IDecoder        *dec;

  bool     visible;
  bool     refresh;
  int      refresh_time;

  Speakers spk;
  Speakers in_spk;
  Speakers out_spk;

  bool     spdif;
  int      spdif_mode;

  int      spdif_pt;
  int      formats;

  int      frames;
  int      errors;

  Speakers old_spk;
  int      old_spdif_mode;
  matrix_t old_matrix;
  char     old_info[1024];

  // Matrix
  DoubleEdit  edt_matrix[NCHANNELS][NCHANNELS];
  // Gain control
  DoubleEdit  edt_master;
  DoubleEdit  edt_gain;
  DoubleEdit  edt_voice;
  DoubleEdit  edt_sur;
  DoubleEdit  edt_lfe;
  // I/O Gains
  DoubleEdit  edt_in_gains[NCHANNELS];
  DoubleEdit  edt_out_gains[NCHANNELS];
  // DRC
  DoubleEdit  edt_drc_power;
  DoubleEdit  edt_drc_level;
  // Bass redirection
  DoubleEdit  edt_bass_freq;
  // Delay
  int         delay_units;
  DoubleEdit  edt_delay[NCHANNELS];
  DoubleEdit  edt_delay_ms;


  AC3FilterDlg(TCHAR *pName, LPUNKNOWN lpunk, HRESULT *phr, int DialogId, int TitleId, int flags);

  BOOL OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  HRESULT OnConnect(IUnknown *pUnknown);
  HRESULT OnDisconnect();
  HRESULT OnActivate();
  HRESULT OnDeactivate();

  void init_controls();
  void set_dynamic_controls();
  void set_matrix_controls();
  void set_cpu_usage();
  void set_controls();

  void command(int control, int message);
};

#endif
