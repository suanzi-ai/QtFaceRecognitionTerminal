#ifndef LED_TASK_HPP
#define LED_TASK_HPP

#include <QObject>
#include <QThread>

namespace suanzi {

class LEDTask : QObject {
  Q_OBJECT
 public:
  static LEDTask* get_instance();
  static bool get_status();

  void turn_on(int mseconds = -1);
  void turn_off(bool force = false);

 private:
  LEDTask(QThread* thread = nullptr, QObject* parent = nullptr);
  ~LEDTask();

  bool is_running_;
  int event_count_;

  bool status_;
};

}  // namespace suanzi
#endif
