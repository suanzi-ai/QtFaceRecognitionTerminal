#ifndef __OBJECT_TEMP_TASK_H
#define __OBJECT_TEMP_TASK_H

#include <QThread>

#include "rs485.hpp"

namespace suanzi {

class DashuTask : QThread {
	
  Q_OBJECT
 public:
  DashuTask(QObject *parent = nullptr);
  ~DashuTask();

 private:
 	void run();

 private slots:
  void tx_temperature(float temperature);

 private:
 	Rs485 *rs485_;
};

}  // namespace suanzi

#endif