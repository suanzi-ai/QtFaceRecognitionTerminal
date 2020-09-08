#ifndef __OBJECT_TEMP_TASK_H
#define __OBJECT_TEMP_TASK_H

#include "temperature_task.hpp"
#include "rs485.hpp"


namespace suanzi {

class DashuTask : public TemperatureTask {

  Q_OBJECT
 public:
  DashuTask(QObject *parent = nullptr);
  ~DashuTask();

 private:
 	void run();

 private:
 	Rs485 *rs485_;
};

}  // namespace suanzi

#endif
