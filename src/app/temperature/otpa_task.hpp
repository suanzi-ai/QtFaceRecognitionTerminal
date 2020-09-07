#ifndef __OTPA_TASK_H
#define __OTPA_TASK_H

#include "otpa16.hpp"
#include "temperature_task.hpp"


namespace suanzi {

class OtpaTask : public TemperatureTask {

  Q_OBJECT
 public:
  OtpaTask(QObject *parent = nullptr);
  ~OtpaTask();

 private:
 	void run();

 private:
 	Otpa16 *otpa16_;
};

}  // namespace suanzi

#endif
