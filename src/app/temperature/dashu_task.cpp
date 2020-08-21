#include "dashu_task.hpp"
#include "logger.hpp"
#include "platform_interface.h"
#include "logger.hpp"

using namespace suanzi;

DashuTask::DashuTask(QObject *parent) {
  rs485_ = new Rs485(1, 9600, 68);
  start();
}

DashuTask::~DashuTask() { delete rs485_; }

void DashuTask::run() {
  const unsigned char send_buf[] = {0x01, 0x03, 0x00, 0x00,
                                    0x00, 0x01, 0x84, 0x0A};
  const size_t recv_len = 7;
  unsigned char recv_buf[recv_len];
  

  Ds_Initialize();
  Ds_OpenUart("/dev/ttyAMA4");

  size_t tick = 0;
  while (true) {
    // Update Ambient Temperature
    if (tick == 0) {
      rs485_->send(send_buf, sizeof(send_buf));
      if (recv_len == rs485_->recv(recv_buf, sizeof(recv_buf))) {
        float ambient_temperature = (recv_buf[3] << 8) + recv_buf[4] / 10.0;
        if (0 != Ds_SetEnvTemperature(ambient_temperature))
          SZ_LOG_WARN("Set ambient temperature failed");
      }
    }

    float temperature = 0.0;
    if (0 == Ds_GetBodyTemperature(&temperature))
      emit tx_temperature(temperature);

    tick = (tick + 1) % 240;
    usleep(250000);
  }
}
