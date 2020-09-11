#ifndef _BOOT_UI_H
#define _BOOT_UI_H

#include <QThread>
#include "vo.h"

namespace suanzi {

class BootUi   : public QThread  {
	
 public:
  BootUi();
  ~BootUi();
  void stop();

 private:
 	void run();

 private:
 	bool b_exit_;
	Vo *pvo_;
};

}  // namespace suanzi

#endif
