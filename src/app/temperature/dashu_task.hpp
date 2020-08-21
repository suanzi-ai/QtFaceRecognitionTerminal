#ifndef __OBJECT_TEMP_TASK_H
#define __OBJECT_TEMP_TASK_H

#include <QThread>

#include "rs485.hpp"
#include "temperature_task.hpp"

namespace suanzi {

class DashuTask : public QThread, public TemperatureTask {
	
  Q_OBJECT
 public:
  DashuTask(QObject *parent = nullptr);
  ~DashuTask();

 private:
 	void run();

 signals:
  void tx_temperature(float temperature);

 private:
 	Rs485 *rs485_;
};

}  // namespace suanzi

#endif