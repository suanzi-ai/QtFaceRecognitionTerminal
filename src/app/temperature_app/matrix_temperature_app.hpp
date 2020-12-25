#ifndef __MATRIX_TEMPERATURE_APP_HPP__
#define __MATRIX_TEMPERATURE_APP_HPP__

#include <QRectF>
#include "temperature_app.hpp"

namespace suanzi {
using namespace io;

class MatrixTemperatureApp : public TemperatureApp {
 public:
  MatrixTemperatureApp(TemperatureTask *temperature_task);
  void read_temperature(const QRectF &face_area, bool valid_face_area) override;

 private:
  bool init();
  bool try_read();
  void init_temperature_area(QRectF &temperature_area);
  void get_temperature_area(const QRectF &face_area, bool valid_face_area,
                            QRectF &temperature_area);
  void get_valid_temperature(std::vector<float> &statistics, float &max_x,
                             float &max_y, float &max_temperature);

  float get_valid_temperature_variance(const std::vector<float> &statistics);
  bool get_face_temperature(const std::vector<float> &statistics,
                            float max_temperature);
  void rotate_temperature_matrix(TemperatureMatrix &mat);

 private:
  TemperatureMatrix temperatures_;
  QRectF temperature_area_;
  float current_variance_;
  float ambient_temperature_;
};

}  // namespace suanzi

#endif
