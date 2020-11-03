#ifndef __TEMPERATURE_TASK_HPP__
#define __TEMPERATURE_TASK_HPP__

#include <QThread>

#include <quface-io/option.hpp>
#include <quface-io/temperature.hpp>

#include "detection_data.hpp"

namespace suanzi {
using namespace io;

class TemperatureTask : public QObject {
  Q_OBJECT

 public:
  static TemperatureTask* get_instance();
  static bool idle();
  static bool ready();

 signals:
  void tx_temperature(float temperature);
  void tx_heatmap(TemperatureMatrix mat, DetectionRatio detection, float x,
                  float y);

 private slots:
  void rx_update(DetectionRatio detection, bool to_clear);

 private:
  TemperatureTask(TemperatureManufacturer m, QThread* thread = nullptr,
                  QObject* parent = nullptr);
  ~TemperatureTask();

 private:
  TemperatureManufacturer m_;
  TemperatureReader::ptr temperature_reader_;

  bool is_running_;

  float ambient_temperature_;
  float face_temperature_;
};

}  // namespace suanzi

#endif
