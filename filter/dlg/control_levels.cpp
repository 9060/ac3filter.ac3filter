#include <math.h>
#include <windows.h>
#include <commctrl.h>
#include "../resource_ids.h"
#include "../resource.h"
#include "control_levels.h"

#define NLEVELS 8

static const int controls[] =
{
  IDC_GRP_LEVELS,
  IDC_LBL_INPUT_LEVELS, IDC_LBL_OUTPUT_LEVELS,
  IDC_GRP_INPUT_LEVELS, IDC_GRP_OUTPUT_LEVELS,

  IDC_LEVEL_IN1, IDC_LEVEL_IN2, IDC_LEVEL_IN3, IDC_LEVEL_IN4, IDC_LEVEL_IN5, IDC_LEVEL_IN6, IDC_LEVEL_IN7, IDC_LEVEL_IN8, 
  IDC_LEVEL_OUT1, IDC_LEVEL_OUT2, IDC_LEVEL_OUT3, IDC_LEVEL_OUT4, IDC_LEVEL_OUT5, IDC_LEVEL_OUT6, IDC_LEVEL_OUT7, IDC_LEVEL_OUT8, 

  0
};

static const int idc_level_in[NLEVELS]  = { IDC_LEVEL_IN1, IDC_LEVEL_IN2, IDC_LEVEL_IN3, IDC_LEVEL_IN4, IDC_LEVEL_IN5, IDC_LEVEL_IN6, IDC_LEVEL_IN7, IDC_LEVEL_IN8 };
static const int idc_level_out[NLEVELS] = { IDC_LEVEL_OUT1, IDC_LEVEL_OUT2, IDC_LEVEL_OUT3, IDC_LEVEL_OUT4, IDC_LEVEL_OUT5, IDC_LEVEL_OUT6, IDC_LEVEL_OUT7, IDC_LEVEL_OUT8 };

static const int ch_map322[NLEVELS] = { CH_LFE, CH_BL, CH_SL, CH_L, CH_C, CH_R, CH_SR, CH_BR };
static const int ch_map52[NLEVELS]  = { CH_LFE, CH_SL, CH_L, CH_CL, CH_C, CH_CR, CH_R, CH_SR };

static const double min_level = -50.0;
static const int ticks = 5;

///////////////////////////////////////////////////////////////////////////////

ControlLevels::ControlLevels(HWND _dlg, IAC3Filter *_filter, IAudioProcessor *_proc, bool _invert_levels): 
Controller(_dlg, ::controls), filter(_filter), proc(_proc), invert_levels(_invert_levels)
{
  filter->AddRef();
  proc->AddRef();
}

ControlLevels::~ControlLevels()
{
  filter->Release();
  proc->Release();
}

void ControlLevels::init()
{
  for (int ch = 0; ch < NLEVELS; ch++)
  {
    SendDlgItemMessage(hdlg, idc_level_in[ch],  PBM_SETBARCOLOR, 0, RGB(0, 128, 0));
    SendDlgItemMessage(hdlg, idc_level_out[ch], PBM_SETBARCOLOR, 0, RGB(0, 128, 0));
    SendDlgItemMessage(hdlg, idc_level_in[ch],  PBM_SETRANGE, 0, MAKELPARAM(0, -min_level * ticks));
    SendDlgItemMessage(hdlg, idc_level_out[ch], PBM_SETRANGE, 0, MAKELPARAM(0, -min_level * ticks));
  }
}

void ControlLevels::update_dynamic()
{
  vtime_t time;
  sample_t input_levels[CH_NAMES];
  sample_t output_levels[CH_NAMES];

  filter->get_playback_time(&time);
  proc->get_levels(time, input_levels, output_levels);

  for (int ch = 0; ch < NLEVELS; ch++)
  {
    long in, out;
    int ch_name = ch_map322[ch];
    if (invert_levels)
    {
      in = input_levels[ch_name]  > 0? long(-value2db(input_levels[ch_name]) * ticks): long(-min_level * ticks);
      out = output_levels[ch_name]  > 0? long(-value2db(output_levels[ch_name]) * ticks): long(-min_level * ticks);
    }
    else
    {
      in = input_levels[ch_name]  > 0? long((value2db(input_levels[ch_name]) - min_level) * ticks): 0;
      out = output_levels[ch_name]  > 0? long((value2db(output_levels[ch_name]) - min_level) * ticks): 0;
    }
    SendDlgItemMessage(hdlg, idc_level_in[ch],  PBM_SETPOS, in, 0);
    SendDlgItemMessage(hdlg, idc_level_out[ch], PBM_SETPOS, out, 0);
    SendDlgItemMessage(hdlg, idc_level_out[ch], PBM_SETBARCOLOR, 0, (output_levels[ch] > 0.99)? RGB(255, 0, 0): RGB(0, 128, 0));
  }
};
