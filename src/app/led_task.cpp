#include "led_task.hpp"

#include <QTimer>

#include <quface-io/engine.hpp>
#include <quface/logger.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

LEDTask* LEDTask::get_instance() {
  static LEDTask instance;
  return &instance;
}

LEDTask::LEDTask(QThread* thread, QObject* parent) : is_running_(false) {
  // Create thread
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

LEDTask::~LEDTask() {}

void LEDTask::turn_on(int mseconds) {
  Engine::instance()->gpio_set(GpioPinLightBox, Config::get_user().enable_led);
  if (mseconds > 0) {
    is_running_ = true;
    QThread::msleep(mseconds);
    turn_off();
    is_running_ = false;
  }
}

void LEDTask::turn_off() {
  if (!is_running_) Engine::instance()->gpio_set(GpioPinLightBox, false);
}
