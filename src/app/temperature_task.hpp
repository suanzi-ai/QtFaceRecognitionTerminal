#ifndef __TEMPERATURE_TASK_HPP__
#define __TEMPERATURE_TASK_HPP__

#include <QRectF>
#include <QThread>
#include <QTimer>
#include <quface-io/option.hpp>
#include <quface-io/temperature.hpp>
#include "detection_data.hpp"
#include "temperature_app.hpp"

namespace suanzi {
using namespace io;

class TemperatureTask : public QObject {
  Q_OBJECT

 public:
  static TemperatureTask *get_instance();
  static bool idle();

 signals:
  void tx_temperature(float temperature);
  void tx_heatmap_init(int success);
  void tx_heatmap(TemperatureMatrix mat, QRectF temperature_area, float x,
                  float y);

 private slots:
  void rx_update(DetectionRatio detection, bool valid_face);

 private:
  TemperatureTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~TemperatureTask();

 private:
  bool is_running_;
  TemperatureApp *temperature_app_;
};

}  // namespace suanzi

#endif
