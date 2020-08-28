#include "random_task.hpp"

#include "logger.hpp"
#include "platform_interface.h"

using namespace suanzi;

RandomTask::RandomTask(QObject *parent) {
  start();
}

void RandomTask::run() {
  while (true) {
    float c = 0.;
    time_t timep;
    struct tm *p;
    time(&timep);
    p = gmtime(&timep);
    int now = p->tm_sec;
    c = (rand() % (3 - 1)) + 1;
    if (now < 10) {
      c = (rand() % (4 - 2)) + 2;  // 0.2~0.3
    } else if (now < 20) {
      c = (rand() % (5 - 3)) + 3;  // 0.3~0.4
    } else if (now < 30) {
      c = (rand() % (6 - 4)) + 4;  // 0.4~0.5
    } else if (now < 40) {
      c = (rand() % (7 - 5)) + 5;  // 0.5~0.6
    } else if (now < 50) {
      c = (rand() % (8 - 6)) + 6;  // 0.6~0.7
    } else if (now < 60) {
      c = (rand() % (9 - 7)) + 7;  // 0.7~0.8
    };
    randomt_ = 36 + c / 10;
    emit tx_temperature(randomt_);
    randomt_ = 36.;
    usleep(250000);
  }
}
