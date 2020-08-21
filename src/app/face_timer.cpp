#include "face_timer.hpp"

#include "config.hpp"
#include "gpio.hpp"

using namespace suanzi;

FaceTimer::FaceTimer(QThread *thread, QObject *parent) {
  // Create thread
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }

  disappear_counter_ = 0;
  disappear_duration_ = 0;
}

FaceTimer::~FaceTimer() {}

void FaceTimer::rx_frame(PingPangBuffer<DetectionData> *buffer) {
  DetectionData *input = buffer->get_pang();

  if (!input->bgr_face_detected() && !input->nir_face_detected()) {
    disappear_counter_++;
    if (disappear_counter_ == Config::get_extract().max_lost_age)
      disappear_begin_ = std::chrono::steady_clock::now();

    if (disappear_counter_ >= Config::get_extract().max_lost_age) {
      auto current_clock = std::chrono::steady_clock::now();
      disappear_duration_ = std::chrono::duration_cast<std::chrono::seconds>(
                                current_clock - disappear_begin_)
                                .count();
      emit tx_face_disappear(disappear_duration_);
      Gpio::set_level(52, false);
    }
  } else {
    if (disappear_counter_ >= Config::get_extract().max_lost_age) {
      emit tx_face_appear(disappear_duration_);
      Gpio::set_level(52, true);

      disappear_counter_ = 0;
      disappear_duration_ = 0;
    }
  }
}