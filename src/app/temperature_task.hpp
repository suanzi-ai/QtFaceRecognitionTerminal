#ifndef __TEMPERATURE_TASK_HPP__
#define __TEMPERATURE_TASK_HPP__

#include <QThread>
#include <quface-io/option.hpp>
#include <quface-io/temperature.hpp>

namespace suanzi {
using namespace io;

class TemperatureTask : public QThread {
  Q_OBJECT

 public:
  TemperatureTask(TemperatureManufacturer m);
  ~TemperatureTask();

 protected:
 signals:
  void tx_temperature(float temperature);

 private slots:
  void rx_enable_read_temperature(bool enable_read_temperature);

 private:
  void run();

 private:
  TemperatureReader::ptr temperature_reader_;
  bool enable_read_temperature_;
};

}  // namespace suanzi

#endif
