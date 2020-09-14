#ifndef __HAIMAN_TASK_H
#define __HAIMAN_TASK_H

#include "temperature_task.hpp"


namespace suanzi {

class HaimanTask : public TemperatureTask {

  Q_OBJECT
 public:
  HaimanTask(QObject *parent = nullptr);
  ~HaimanTask();

 private:
 	void run();
	unsigned char get_check_sum(unsigned char *buf, int len);

 private:
 	
};

}  // namespace suanzi

#endif
