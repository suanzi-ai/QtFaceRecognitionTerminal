#include "otpa_task.hpp"

#include <quface/logger.hpp>

using namespace suanzi;

OtpaTask::OtpaTask(QObject *parent) {
  otpa16_ = new Otpa16();
  start();
}

OtpaTask::~OtpaTask() { delete otpa16_; }

void OtpaTask::run() {
  OtpaTemperatureData otpa_temp;
  while (true) {

    if (enable_read_temperature_) {
	    float temperature = 0.0;
	    if (otpa16_->read_temperature(&otpa_temp)) {
	      temperature = otpa_temp.pixel_temp[otpa_temp.max_pixel_temp_index];
		  printf("temp=%.2f\n", temperature);
	      if (temperature > 30 && temperature < 36)
	        temperature = 36 + temperature - (int)temperature;
	      emit tx_temperature(temperature);
	    }
    }
    usleep(250000);
  }
}
