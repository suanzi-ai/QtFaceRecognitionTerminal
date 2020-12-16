#ifndef __TEMPERATURE_APP_HPP__
#define __TEMPERATURE_APP_HPP__

#include <QRectF>
#include <quface-io/temperature.hpp>

namespace suanzi {

using namespace io;

class TemperatureTask;

class TemperatureApp {
 public:
  TemperatureApp(TemperatureTask *temperature_task);
  ~TemperatureApp();
  virtual void read_temperature(const QRectF &face_area,
                                bool valid_face_area) = 0;

 protected:
  bool init();

 protected:
  TemperatureTask *temperature_task_;
  TemperatureReader::ptr temperature_reader_;
};

}  // namespace suanzi

#endif
