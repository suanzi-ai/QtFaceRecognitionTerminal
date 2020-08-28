#ifndef __OTPA_TASK_H
#define __OTPA_TASK_H

#include "otpa16.hpp"
#include "temperature_task.hpp"
#include <deque>

namespace suanzi {

class OtpaTask : public TemperatureTask {

  Q_OBJECT
 public:
  OtpaTask(QObject *parent = nullptr);
  ~OtpaTask();

 private:
 	void run();
	void calculate_temperature(float cur_temp);

 private:
 	Otpa16 *otpa16_;
	std::deque<float> temperatures_;
	float cur_max_temp_;
	float cur_min_temp_;
};

}  // namespace suanzi

#endif