#include "face_timer.hpp"

#include <quface-io/engine.hpp>

#include "config.hpp"
#include "led_task.hpp"

using namespace suanzi;
using namespace suanzi::io;

FaceTimer *FaceTimer::get_instance() {
  static FaceTimer instance;
  return &instance;
}

FaceTimer::FaceTimer(QObject *parent) : QThread(parent) {
  screen_saver_timer_ = new QTimer(this);
  screen_saver_timer_->setSingleShot(true);
  connect(screen_saver_timer_, SIGNAL(timeout()), this,
          SLOT(screen_saver_timeout()));

  white_led_timer_ = new QTimer(this);
  white_led_timer_->setSingleShot(true);
  white_led_timer_->setInterval(MAX_WHITE_LED_TIMEOUT * 1000);
  connect(white_led_timer_, SIGNAL(timeout()), this, SLOT(white_led_timeout()));

  moveToThread(this);
  start();
}

FaceTimer::~FaceTimer() {}

void FaceTimer::rx_detect_result(bool valid_detect) {
  static bool has_face = true;

  // on has_face changes
  if (has_face != valid_detect) {
    has_face = valid_detect;

    if (has_face) {
      LEDTask::get_instance()->turn_on();
      emit tx_display_screen_saver(false);
      screen_saver_timer_->stop();
      white_led_timer_->stop();
    } else {
      screen_saver_timer_->start(Config::get_user().screensaver_timeout * 1000);
      white_led_timer_->start();
    }
  }
}

void FaceTimer::screen_saver_timeout() {
  if (Config::get_user().enable_screensaver) {
    emit tx_display_screen_saver(true);
  }
}

void FaceTimer::white_led_timeout() {
  LEDTask::get_instance()->turn_off();
  emit tx_reset();
}

void FaceTimer::run() { exec(); }
