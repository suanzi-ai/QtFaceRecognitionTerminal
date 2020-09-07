#include "temperature_task.hpp"

using namespace suanzi;

void TemperatureTask::rx_enable_read_temperature(bool enable_read_temperature) {
	enable_read_temperature_ = enable_read_temperature;
}

