#ifndef _AMBIENT_TEMP_TASK_H
#define _AMBIENT_TEMP_TASK_H

#include <QThread>
#include "rs485.hpp"

namespace suanzi {

class AmbientTempTask : QThread {

  Q_OBJECT
  	
 public:
  AmbientTempTask(QObject *parent = nullptr);
  ~AmbientTempTask();

 private:
	void run();
	void rx_ambient_temp_run();
	
 signals:
  void tx_ambient_temp(float temp);

 private:
 	Rs485 *rs485_;
};

}  // namespace suanzi

#endif