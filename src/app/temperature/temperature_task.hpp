#ifndef __TEMPERATURE_TASK_HPP__
#define __TEMPERATURE_TASK_HPP__

#include <QThread>

namespace suanzi {

class TemperatureTask : QThread {
	
  Q_OBJECT
 public:
  TemperatureTask(QObject *parent = nullptr);
  ~TemperatureTask();

 private:
  void run();

 private slots:
  void tx_temperature(float temperature);
  
};

}  // namespace suanzi

#endif