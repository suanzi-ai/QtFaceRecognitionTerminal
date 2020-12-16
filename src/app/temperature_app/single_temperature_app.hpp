#ifndef __SINGLE_TEMPERATURE_APP_HPP__
#define __SINGLE_TEMPERATURE_APP_HPP__

#include <QRectF>
#include "temperature_app.hpp"

namespace suanzi {
using namespace io;

class TemperatureTask;

class SingleTemperatureApp : public TemperatureApp {
 public:
  SingleTemperatureApp(TemperatureTask *temperature_task);
  void read_temperature(const QRectF &face_area, bool valid_face_area) override;

 private:
};

}  // namespace suanzi

#endif
