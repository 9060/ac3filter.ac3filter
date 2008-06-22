/*
  Represent a list of controllers as a single controller
*/

#ifndef CONTROL_LIST_H
#define CONTROL_LIST_H

#include <memory>
#include <vector>
#include "../guids.h"
#include "../controls.h"

class ControlList : public Controller
{
protected:
  typedef std::auto_ptr<Controller> PCtrl;
  typedef std::vector<PCtrl> VCtrl;
  VCtrl ctrl;

public:
  ControlList(HWND dlg);

  void add(Controller *ctrl);
  void drop();

  virtual void init();
  virtual void update();
  virtual void update_dynamic();
  virtual bool own_control(int control);
  virtual cmd_result command(int control, int message);
};

#endif
