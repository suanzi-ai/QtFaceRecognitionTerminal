#ifndef __TEMPERATURE_TASK_HPP__
#define __TEMPERATURE_TASK_HPP__

#include <QThread>

namespace suanzi {

class TemperatureTask : public QThread {

  Q_OBJECT

protected:

 signals:
  void tx_temperature(float temperature);

  
private slots:
	void rx_enable_read_temperature(bool enable_read_temperature);

protected:
	bool enable_read_temperature_;
};

}  // namespace suanzi

#endif
