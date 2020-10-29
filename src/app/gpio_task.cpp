#include "gpio_task.hpp"

#include <QTimer>

#include <quface-io/engine.hpp>
#include <quface/logger.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

GPIOTask* GPIOTask::get_instance() {
  static GPIOTask instance;
  return &instance;
}

GPIOTask::GPIOTask(QThread* thread, QObject* parent) : event_count_(0) {
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

GPIOTask::~GPIOTask() {}

void GPIOTask::rx_trigger(PersonData person, bool if_duplicated) {
  auto user = Config::get_user();

  bool open_door = true;
  if (user.door_open_cond & DoorOpenCond::Status)
    open_door = open_door && person.is_status_normal();
  if (user.enable_temperature &&
      (user.door_open_cond & DoorOpenCond::Temperature))
    open_door = open_door && person.is_temperature_normal();

  // Open door GPIO
  if (open_door) {
    Engine::instance()->gpio_set(GpioPinDOOR, !user.door_unopen_state);
    event_count_ += 1;
    QTimer::singleShot(user.door_open_timeout, this, SLOT(rx_reset()));
  }
}

void GPIOTask::rx_reset() {
  if (--event_count_ <= 0) {
    event_count_ = 0;
    Engine::instance()->gpio_set(GpioPinDOOR,
                                 Config::get_user().door_unopen_state);
  }
}
