#ifndef GPIO_TASK_HPP
#define GPIO_TASK_HPP

#include <QObject>
#include <QThread>

#include "person_service.hpp"

namespace suanzi {

class GPIOTask : QObject {
  Q_OBJECT
 public:
  static GPIOTask* get_instance();

 private slots:
  void rx_trigger(PersonData person, bool audio_duplicated,
                  bool record_duplicated);
  void rx_reset();

 private:
  GPIOTask(QThread* thread = nullptr, QObject* parent = nullptr);
  ~GPIOTask();

  int event_count_;
};

}  // namespace suanzi

#endif
