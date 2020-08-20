#include "dashu_task.hpp"

#include "platform_interface.h"

using namespace suanzi;

DashuTask::DashuTask(QObject *parent) {
  rs485_ = new Rs485(1, 9600, 68);
  start();
}

DashuTask::~DashuTask() { delete rs485_; }

void DashuTask::run() {
  const unsigned char send_buf[] = {0x01, 0x03, 0x00, 0x00,
                                    0x00, 0x01, 0x84, 0x0A};
  unsigned char recv_buf[7];
  const size_t recv_len = 7;

  Ds_Initialize();
  Ds_OpenUart("/dev/ttyAMA4");

  size_t tick = 0;
  while (true) {
    // Update Ambient Temperature
    if (tick == 0) {
      rs485_->send(send_buf, sizeof(send_buf));
      if (recv_len == rs485_->recv(recv_buf, recv_len)) {
        float ambient_temperature = (recv_buf[3] << 8) + recv_buf[4] / 10.0;
        if (0 != Ds_SetEnvTemperature(ambient_temperature))
          SZ_LOG_WARN("Set ambient temperature failed");
      }
    }

    float temperature = 0.0;
    if (0 == Ds_GetBodyTemperature(&temperature))
      emit tx_temperature(temperature);

    tick = (tick + 1) % 15;
    usleep(2000000);
  }
}
