#ifndef AUDIO_TASK_HPP
#define AUDIO_TASK_HPP

#include <QObject>

#include "audio.hpp"
#include "person_service.hpp"

namespace suanzi {

class AudioTask : QObject {
  Q_OBJECT
 public:
  AudioTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~AudioTask();

 private slots:
  void rx_display(PersonData person, bool if_duplicated);
  void rx_reset();

 private:
  AudioPlayer player_;
  bool if_playing_;
};

}  // namespace suanzi

#endif