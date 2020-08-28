#include "otpa_task.hpp"
#include "logger.hpp"

using namespace suanzi;

OtpaTask::OtpaTask(QObject *parent) {
  otpa16_ = new Otpa16();
  start();
}

OtpaTask::~OtpaTask() { delete otpa16_; }

void OtpaTask::run() {
  OtpaTemperatureData otpa_temp;
  while (true) {
    // Update Ambient Temperature
    float temperature = 0.0;
    if (otpa16_->read_temperature(&otpa_temp)) {
      // printf("max temp:%.2f\n",
      // otpa_temp.pixel_temp[otpa_temp.max_pixel_temp_index]);
      if (temperature > 30 && temperature < 36)
        temperature = 35 + temperature - (int)temperature;
      temperature = otpa_temp.pixel_temp[otpa_temp.max_pixel_temp_index];

      emit tx_temperature(temperature);
    }
    usleep(250000);
  }
}
