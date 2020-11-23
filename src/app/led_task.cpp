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

bool LEDTask::get_status() { return get_instance()->status_; }

LEDTask::LEDTask(QThread* thread, QObject* parent)
    : is_running_(false), event_count_(0), status_(false) {
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
  status_ = Config::get_user().enable_led;
  event_count_++;
  if (mseconds > 0) {
    QThread::msleep(mseconds);
    if (--event_count_ == 0) turn_off(true);
  }
  if (mseconds <= 0) is_running_ = true;
}

void LEDTask::turn_off(bool force) {
  if (force) {
    Engine::instance()->gpio_set(GpioPinLightBox, false);
    status_ = false;
  }
  else if (is_running_) {
    Engine::instance()->gpio_set(GpioPinLightBox, false);
    is_running_ = status_ = false;
    event_count_ = 0;
  }
}
