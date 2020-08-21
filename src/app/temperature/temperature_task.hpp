#ifndef __TEMPERATURE_TASK_HPP__
#define __TEMPERATURE_TASK_HPP__

#include <QThread>

namespace suanzi {

class TemperatureTask : public QThread {
	
  Q_OBJECT

protected:

 signals:
  void tx_temperature(float temperature);

};

}  // namespace suanzi

#endif