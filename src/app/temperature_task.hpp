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
  static TemperatureTask* get_instance();

 signals:
  void tx_temperature(float temperature);

 private slots:
  void rx_enable();
  void rx_disable();

 private:
  TemperatureTask(TemperatureManufacturer m);
  ~TemperatureTask();

  void run();

 private:
  TemperatureReader::ptr temperature_reader_;
  bool is_enabled_;
};

}  // namespace suanzi

#endif
