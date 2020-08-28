#include "otpa_task.hpp"
#include "logger.hpp"

using namespace suanzi;

OtpaTask::OtpaTask(QObject *parent) {
  otpa16_ = new Otpa16();
  cur_max_temp_ = 0;
  cur_min_temp_ = 1000;
  start();
}

OtpaTask::~OtpaTask() { delete otpa16_; }

void OtpaTask::calculate_temperature(float cur_temp) {
	if (temperatures_.size() > 30) {
		temperatures_.pop_front();
	}
	temperatures_.push_back(cur_temp);
	int len = temperatures_.size();
	float temp = 0.0;
	for (int i = 0; i < len; i++) {
		temp += temperatures_[i];
	}
	temp /= len;
	emit tx_temperature(temp);
}

void OtpaTask::run() {
  OtpaTemperatureData otpa_temp;
  while (true) {
    // Update Ambient Temperature
    float temperature = 0.0;
	if (otpa16_->read_temperature(&otpa_temp)) {
		//printf("max temp:%.2f\n", otpa_temp.pixel_temp[otpa_temp.max_pixel_temp_index]);
		calculate_temperature(otpa_temp.pixel_temp[otpa_temp.max_pixel_temp_index]);
	}
    usleep(250000);
  }
}
