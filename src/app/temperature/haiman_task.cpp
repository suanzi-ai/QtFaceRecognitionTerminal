#include "haiman_task.hpp"
#include <quface/logger.hpp>
#include "platform_interface.h"
#include "uart.hpp"

using namespace suanzi;

HaimanTask::HaimanTask(QObject *parent) {
  start();
}

HaimanTask::~HaimanTask() {  }


unsigned char HaimanTask::get_check_sum(unsigned char *buf, int len) {
	unsigned char sum = 0;
	for (int i = 0; i < len; i++) {
		sum += buf[i];
	}
	return sum;
}

void HaimanTask::run() {
  const unsigned char send_buf[] = {0xA5, 0x55, 0x01, 0xFB};
  const size_t recv_len = 7;
  unsigned char recv_buf[recv_len];

  Uart uart(4, 115200);
  size_t tick = 0;
  while (true) {
    // Update Ambient Temperature
    float temperature = 0.0;
    if (uart.send(send_buf, sizeof(send_buf))) {
	  int timeout = 0;
	  int recv_len = 0;
	  recv_len = uart.recv(recv_buf, 7);
	  printf("len=%d  ", recv_len);
	  if (recv_len == 7 && recv_buf[0] == 0xA5 && recv_buf[6] == get_check_sum(recv_buf, 6)) {
		 temperature = (recv_buf[2] + (recv_buf[3] << 8)) / 100.0;
		 printf("temp=%.2f ", temperature);
		 if (temperature < 0.00001 && temperature >= -0.0001)
		 	;
		 else
			emit tx_temperature(temperature);		 	
	  }
	  printf("\n");
    }

    usleep(1000000);
  }
}
