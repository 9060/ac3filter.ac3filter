#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <windows.h>
#include <commctrl.h>   // tooltip
#include "controls.h"
#include "wincomp.h"

#define TTS_BALLOON 0x40

///////////////////////////////////////////////////////////////////////////////
// Tooltip
///////////////////////////////////////////////////////////////////////////////

Tooltip::Tooltip()
{
  hwnd = 0;
  tooltip = 0;
  hinstance = 0;

  enabled = true;
  visible = false;
  delay = 1000;
};

Tooltip::~Tooltip()
{
  destroy();
};


bool
Tooltip::create(HINSTANCE _hinstance, HWND _hwnd, bool _enabled)
{
  destroy();

  // create tooltip control

  tooltip = CreateWindowEx(
    WS_EX_TOPMOST, TOOLTIPS_CLASS, "AC3Filter tooltips", 
    WS_POPUP | TTS_ALWAYSTIP | TTS_NOPREFIX | TTS_BALLOON,
    CW_USEDEFAULT, CW_USEDEFAULT, 
    CW_USEDEFAULT, CW_USEDEFAULT,
    _hwnd, 0, _hinstance, 0);

  if (tooltip == 0)
    return false;

  hwnd = _hwnd;
  hinstance = _hinstance;
  enable(_enabled);
  return true;
}

void
Tooltip::destroy()
{
  if (tooltip)
    DestroyWindow(tooltip);

  hwnd = 0;
  tooltip = 0;
  hinstance = 0;

  enabled = false;
  visible = false;
}

void
Tooltip::enable(bool _enabled)
{
  enabled = _enabled;
  SendMessage(tooltip, TTM_ACTIVATE, enabled, 0);
}

void
Tooltip::set_width(int _width)
{
  SendMessage(tooltip, TTM_SETMAXTIPWIDTH, 0, _width);
}

void
Tooltip::set_delay(int _ms)
{
  delay = _ms;
}

void
Tooltip::show(bool _visible)
{
  visible = _visible;
  SendMessage(tooltip, TTM_TRACKACTIVATE, FALSE, 0);
  if (visible)
  {
    // WindowFromPoint ignores static controls and disabled windows.
    // Threrfore, we have to use ChildWindowFromPoint that does not ignore
    // static controls and disabled windows.

    HWND mouse_hwnd = WindowFromPoint(mouse_pt);
    HWND temp_hwnd = mouse_hwnd;
    if (mouse_hwnd)
    {
      POINT child_pt = mouse_pt;
      if (ScreenToClient(mouse_hwnd, &child_pt))
      {
        HWND child_hwnd = ChildWindowFromPoint(mouse_hwnd, child_pt);
        if (child_hwnd)
          mouse_hwnd = child_hwnd;
      }

      TOOLINFO ti;
      memset(&ti, 0, sizeof(ti));
      ti.cbSize = sizeof(ti);
      ti.hwnd = hwnd;
      ti.uId = (UINT)mouse_hwnd;

      SendMessage(tooltip, TTM_TRACKPOSITION, 0, MAKELPARAM(mouse_pt.x, mouse_pt.y));
      SendMessage(tooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
    }
  }
}

void
Tooltip::track()
{
  if (!enabled)
    return;

  POINT pt;
  GetCursorPos(&pt);

  __int64 time;
  SYSTEMTIME systime;
  GetSystemTime(&systime);
  SystemTimeToFileTime(&systime, (FILETIME*)&time);

  if (mouse_pt.x == pt.x && mouse_pt.y == pt.y)
  {
    if (!visible && time - mouse_time > delay * 10000)
     show(true);
  }
  else
  {
    mouse_pt = pt;
    mouse_time = time;
    if(visible)
      show(false);
  }
}

void
Tooltip::add_window(HWND window, const char *text)
{
  TOOLINFO ti;
  memset(&ti, 0, sizeof(ti));
  ti.cbSize = sizeof(ti);
  ti.uFlags = 0;
  ti.hwnd = hwnd;
  ti.hinst = hinstance;
  ti.uId = (UINT)window;
  ti.lpszText = (LPTSTR)text;
  SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
}

void
Tooltip::add_control(int control_id, const char *text)
{
  HWND control = GetDlgItem(hwnd, control_id);
  if (control)
    add_window(control, text);
/*
  {
    RECT dlg_rect;
    RECT control_rect;
    GetWindowRect(hwnd, &dlg_rect);
    GetWindowRect(control, &control_rect);

    TOOLINFO ti;
    memset(&ti, 0, sizeof(ti));
    ti.cbSize = sizeof(ti);
    ti.uFlags = 0;
    ti.hwnd = hwnd;
    ti.hinst = hinstance;
    ti.uId = (UINT)control;
    ti.lpszText = (LPTSTR)text;
    ti.rect.left = control_rect.left - dlg_rect.left;
    ti.rect.right = control_rect.right - dlg_rect.left;
    ti.rect.top = control_rect.top - dlg_rect.top;
    ti.rect.bottom = control_rect.bottom - dlg_rect.top;

    SendMessage(tooltip, TTM_ADDTOOL, 0, (LPARAM)&ti);
  }
*/
}

///////////////////////////////////////////////////////////////////////////////
// SubclassedControl
///////////////////////////////////////////////////////////////////////////////

void
SubclassedControl::link(HWND _dlg, int _item)
{
  if (hwnd) unlink();

  dlg = _dlg;
  item = _item;
  hwnd = GetDlgItem(_dlg, _item);
  if (hwnd)
  {
    next_wndproc = (WNDPROC) SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)SubClassProc);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)(SubclassedControl *)this);
  }
  on_link();
}

void 
SubclassedControl::unlink()
{
  on_unlink();
  if (hwnd)
  {
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)next_wndproc);
    SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
  }

  dlg = 0;
  item = 0;
  hwnd = 0;
}

void
SubclassedControl::enable(bool enabled)
{
  EnableWindow(hwnd, enabled);
}

LRESULT CALLBACK
SubclassedControl::SubClassProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  SubclassedControl *iam = (SubclassedControl *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
  if (iam) return iam->wndproc(hwnd, msg, wParam, lParam);
  else return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Edit, IntEdit, DoubleEdit, TextEdit controls
///////////////////////////////////////////////////////////////////////////////

LRESULT CALLBACK
Edit::wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) 
  { 
    case WM_GETDLGCODE:
      return SubclassedControl::wndproc(hwnd, msg, wParam, lParam) | DLGC_WANTALLKEYS;

    case WM_KILLFOCUS:
      if (editing)
      {
        editing = false;
        if (read_value())
        {
          write_value();
          SendMessage(dlg, WM_COMMAND, MAKEWPARAM(item, CB_ENTER), 0); 
        }
        else
        {
          restore_value();
          write_value();
          MessageBox(dlg, incorrect_value(), "Error", MB_ICONEXCLAMATION | MB_OK);
        }
      }
      break;

    case WM_KEYDOWN: 
      if (!editing)
      {
        backup_value();
        editing = true;
      } 

      if (editing && wParam == VK_RETURN)
      {
        editing = false;
        if (read_value())
        {
          write_value();
          SendMessage(dlg, WM_COMMAND, MAKEWPARAM(item, CB_ENTER), 0); 
        }
        else
        {
          restore_value();
          write_value();
          MessageBox(dlg, incorrect_value(), "Error", MB_ICONEXCLAMATION | MB_OK);
        }
        return 0; 
      }

      if (editing && wParam == VK_ESCAPE)
      {
        restore_value();
        write_value();
        editing = false;
        return 0;
      }

      break;
                            
    case WM_KEYUP: 
    case WM_CHAR: 
      switch (wParam) 
      { 
        case VK_RETURN: 
        case VK_ESCAPE: 
          return 0; 
      }
      break;

  } // switch  (msg)

  // Call the original window procedure for default processing. 
  return SubclassedControl::wndproc(hwnd, msg, wParam, lParam);
}

bool
IntEdit::read_value()
{
  char buf[256];

  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, 256, (LPARAM) buf))
  {
    value = 0;
    return true;
  }

  int new_value = 0;
  char tmp;
  if (sscanf(buf, "%i%c", &new_value, &tmp) != 1)
    return false;

  value = new_value;
  return true;
}

void
IntEdit::backup_value()
{
  old_value = value;
}

void 
IntEdit::restore_value()
{
  value = old_value;
}

void 
IntEdit::write_value()
{
  char buf[256];
  sprintf(buf, "%i", value);
  SetDlgItemText(dlg, item, buf);
}

bool
DoubleEdit::read_value()
{
  char buf[256];

  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, 256, (LPARAM) buf))
  {
    value = 0;
    return true;
  }

  double new_value = 0.0;
  char tmp;
  if (sscanf(buf, "%lg%c", &new_value, &tmp) != 1)
    return false;

  value = new_value;
  return true;
}

void
DoubleEdit::backup_value()
{
  old_value = value;
}

void 
DoubleEdit::restore_value()
{
  value = old_value;
}

void 
DoubleEdit::write_value()
{
  char buf[256];
  sprintf(buf, "%.4g", value);
  SetDlgItemText(dlg, item, buf);
}

TextEdit::TextEdit(size_t _size)
{
  size = 0;
  value = new char[_size+1];
  old_value = new char[_size+1];
  value[0] = 0;
  old_value[0] = 0;

  if (value && old_value)
    size = _size;
}

TextEdit::~TextEdit()
{
  if (value) delete value;
  if (old_value) delete old_value;
}

bool
TextEdit::read_value()
{
  if (!SendDlgItemMessage(dlg, item, WM_GETTEXT, size, (LPARAM) value))
    value[0] = 0;
  else
    value[size] = 0;

  return true;
}

void
TextEdit::backup_value()
{
  strcpy(old_value, value);
}

void 
TextEdit::restore_value()
{
  strcpy(value, old_value);
}

void 
TextEdit::write_value()
{
  SetDlgItemText(dlg, item, value);
}

void 
TextEdit::set_text(const char *_text)
{
  strncpy(value, _text, size);
  value[size] = 0;
  write_value();
}

///////////////////////////////////////////////////////////////////////////////
// LinkButton
///////////////////////////////////////////////////////////////////////////////

void
LinkButton::on_link()
{
  // Create underlined font
  HFONT dlg_font = (HFONT)SendDlgItemMessage(dlg, item, WM_GETFONT, 0, 0);
  LOGFONT logfont;
  GetObject(dlg_font, sizeof(LOGFONT), &logfont);
  logfont.lfUnderline = TRUE;
  font = CreateFontIndirect(&logfont);
}

void 
LinkButton::on_unlink()
{
  DeleteObject(font);
  font = 0;
}

LRESULT CALLBACK 
LinkButton::wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg) 
  { 
    case WM_PAINT: 
    {
      PAINTSTRUCT ps;
      HDC dc = BeginPaint(hwnd, &ps);
      paint(dc);
      EndPaint(hwnd, &ps);
      return 0;
    }
    case WM_LBUTTONDOWN:
    {
      press();
      return 0;
    }
  } 

  // Call the original window procedure for default processing. 
  return SubclassedControl::wndproc(hwnd, msg, wParam, lParam);
}

void 
LinkButton::paint(HDC dc)
{
  RECT client_rect;
  GetClientRect(hwnd, &client_rect);

  int i;
  char link_text[256];
  int link_text_len = GetWindowText(hwnd, link_text, 256);

  // find description text
  for (i = 0; i < link_text_len; i++)
    if (link_text[i] == '|')
      break;

  link_text_len = i;

  // Draw description text or url
  HFONT old_font = (HFONT)SelectObject(dc, font);
  COLORREF old_color = SetTextColor(dc, RGB(0, 0, 255));
  DrawText(dc, link_text, link_text_len, &client_rect, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
  SetTextColor(dc, old_color);
  SelectObject(dc, old_font);
}

void 
LinkButton::press()
{
  int i;
  char link_text[256];
  int link_text_len = GetWindowText(hwnd, link_text, 256);

  // find url
  for (i = 0; i < link_text_len; i++)
    if (link_text[i] == '|')
    {
      i++;
      break;
    }

  // execute url
  if (i < link_text_len)
    ShellExecute(hwnd, 0, link_text + i, 0, 0, SW_SHOWMAXIMIZED);
  else
    ShellExecute(hwnd, 0, link_text, 0, 0, SW_SHOWMAXIMIZED);
}
