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

  bool switch_relay = true;
  if (user.relay_switch_cond & RelaySwitchCond::Status)
    switch_relay = switch_relay && person.is_status_normal();
  if (user.enable_temperature &&
      (user.relay_switch_cond & RelaySwitchCond::Temperature))
    switch_relay = switch_relay && person.is_temperature_normal();

  // Open door GPIO
  if (switch_relay) {
    if (user.relay_default_state == RelayState::Low) {
      Engine::instance()->gpio_set(GpioPinDOOR, true);
    } else {
      Engine::instance()->gpio_set(GpioPinDOOR, false);
    }
    event_count_ += 1;
    QTimer::singleShot(user.relay_restore_time * 1000, this, SLOT(rx_reset()));
  }
}

void GPIOTask::rx_reset() {
  if (--event_count_ <= 0) {
    event_count_ = 0;
    if (Config::get_user().relay_default_state == RelayState::Low) {
      Engine::instance()->gpio_set(GpioPinDOOR, false);
    } else {
      Engine::instance()->gpio_set(GpioPinDOOR, true);
    }
  }
}
