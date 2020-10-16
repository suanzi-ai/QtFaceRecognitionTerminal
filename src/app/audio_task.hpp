#ifndef AUDIO_TASK_HPP
#define AUDIO_TASK_HPP

#include <mutex>
#include <vector>

#include <QObject>
#include <QThread>

#include "person_service.hpp"

namespace suanzi {

struct Audio {
  std::vector<SZ_BYTE> data;
  int duration;
};

class AudioTask : QObject {
  Q_OBJECT
 public:
  AudioTask(QThread* thread = nullptr, QObject* parent = nullptr);
  ~AudioTask();

 private slots:
  void rx_report(PersonData person);
  void rx_warn();

 signals:
  void tx_finish();
  void tx_warn_finish();

 private:
  void load_audio();
  bool read_audio(const std::string& name, Audio& audio);
  void play_audio(Audio& audio);

  Audio success_audio_;
  Audio warn_audio_;
  Audio fail_audio_;
  Audio temp_normal_audio_;
  Audio temp_abnormal_audio_;

  bool is_reporting_;
};

}  // namespace suanzi

#endif
