#include "ambient_temp_task.hpp"
#include <thread>
#include <functional>
#include <unistd.h>
#include "logger.hpp"


using namespace suanzi;

AmbientTempTask::AmbientTempTask(QObject *parent) {
  rs485_ = new Rs485(1, 9600, 68);
  start();
  static std::thread th(std::bind(&AmbientTempTask::rx_ambient_temp_run, this));
}

AmbientTempTask::~AmbientTempTask() { delete rs485_;}


void AmbientTempTask::run() {
	const unsigned char send_buf[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A};
	while(1) {
		printf("read ambient:\n");
		rs485_->send(send_buf, sizeof(send_buf));
		usleep(30000000);
	}
}

void AmbientTempTask::rx_ambient_temp_run() {
	unsigned char recv_buf[7];
	while(1) {
		int len = rs485_->recv(recv_buf, 7); 
		if (7 == len) {
			long temp = (recv_buf[3] << 8) + recv_buf[4];
			float ambient_temp = temp / 10.0;
			emit tx_ambient_temp(ambient_temp);
			printf("rx ambient %.3f \n", ambient_temp);
		}
	}
}




