#ifndef FACE_TIMER_HPP
#define FACE_TIMER_HPP

#include <QObject>
#include <QThread>
#include <QTimer>
#include <chrono>

#include "detection_data.hpp"
#include "pingpang_buffer.hpp"
#include "quface_common.hpp"

namespace suanzi {
class FaceTimer : QThread {
  Q_OBJECT
 public:
  static FaceTimer *get_instance();

 private slots:
  void rx_detect_result(bool valid_detect);
  void screen_saver_timeout();
  void white_led_timeout();

 signals:
  void tx_display_screen_saver(bool visible);
  void tx_reset();

 private:
  FaceTimer(QObject *parent = nullptr);
  ~FaceTimer();
  void display_screen_saver(bool visible);
  void run() override;

  QTimer *screen_saver_timer_;
  QTimer *white_led_timer_;

  const int MAX_WHITE_LED_TIMEOUT = 1;
};

}  // namespace suanzi

#endif
