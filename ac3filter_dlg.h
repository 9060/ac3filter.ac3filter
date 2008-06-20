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
  static CUnknown * WINAPI CreateEq    (LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateSPDIF (LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateSystem(LPUNKNOWN lpunk, HRESULT *phr);
  static CUnknown * WINAPI CreateAbout (LPUNKNOWN lpunk, HRESULT *phr);

  void reload_state();

private:
  const char      *title;
  IAC3Filter      *filter;
  IDecoder        *dec;
  IAudioProcessor *proc;

  bool visible;
  bool refresh;

  // Dialog controls controllers
  Controller *ctrl;
  Controller *cpu;

  // interface options
  bool     tray;
  bool     tooltips;
  bool     invert_levels;
  int      refresh_time;

  // input-output format
  Speakers in_spk;
  Speakers old_in_spk;

  // Language
  char        lang[LANG_LEN];
  // Interface
  DoubleEdit  edt_refresh_time;
  // Links
  LinkButton  lnk_donate;

  // Tooltip control
  Tooltip tooltip_ctl;

  AC3FilterDlg(TCHAR *pName, LPUNKNOWN lpunk, HRESULT *phr, int DialogId, const char *title_id, const char *title_def);

  BOOL OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

  STDMETHODIMP GetPageInfo(LPPROPPAGEINFO pPageInfo);
  HRESULT OnConnect(IUnknown *pUnknown);
  HRESULT OnDisconnect();
  HRESULT OnActivate();
  HRESULT OnDeactivate();

  /////////////////////////////////////////////////////////////////////////////
  // Interface options

  bool get_tooltips();
  void set_tooltips(bool tooltips);

  bool get_invert_levels();
  void set_invert_levels(bool invert_levels);

  int get_refresh_time();
  void set_refresh_time(int refresh_time);

  /////////////////////////////////////////////////////////////////////////////
  // Interface update functions
  // 
  // * init()
  //   Top-level function to init the interface.
  //   Translate and init controls.
  //
  // * update()
  //   Top-level function to update controls.
  //   Reload state and update static and dynamic controls.
  //
  // * init_controls()
  //   Set initial controls parameters (range, color, ets), bind class members
  //   to controls, etc.
  //
  // * init_tooltips()
  //   Init and translate tooltips.
  //
  // * translate_controls()
  //   Translate control labels, create and translate tips.
  //
  // * update_dynamic_controls()
  //   Update only dynamic controls. Update the matrix if auto_matrix is on.
  //
  // * update_static_controls()
  //   Update only static controls. Update the matrix if auto_matrix is off.

  void init();
  void update();

  void init_controls();
  void init_tooltips();
  void translate_controls();

  void update_dynamic_controls();
  void update_static_controls();

  /////////////////////////////////////////////////////////////////////////////
  // Handle control notifications

  void command(int control, int message);
};

#endif
