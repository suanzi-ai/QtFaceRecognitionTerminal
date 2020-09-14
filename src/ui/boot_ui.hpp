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
  bool load_screen_type(int &lcd_screen_type, int &width, int &height);

 private:
 	void run();

 private:
 	bool b_exit_;
	Vo *pvo_;
	int width_;
	int height_;
};

}  // namespace suanzi

#endif
