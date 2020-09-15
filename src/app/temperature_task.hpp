#ifndef __TEMPERATURE_TASK_HPP__
#define __TEMPERATURE_TASK_HPP__

#include <QThread>
#include <quface-io/option.hpp>

namespace suanzi {

class TemperatureTask : public QThread {
  Q_OBJECT

 public:
  TemperatureTask(io::TemperatureManufacturer p);
  ~TemperatureTask();

 protected:
 signals:
  void tx_temperature(float temperature);

 private slots:
  void rx_enable_read_temperature(bool enable_read_temperature);

 private:
  void run();

 protected:
  bool enable_read_temperature_;
};

}  // namespace suanzi

#endif
