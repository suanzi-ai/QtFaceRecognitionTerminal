#ifndef AUDIO_TASK_HPP
#define AUDIO_TASK_HPP

#include <QObject>
#include <vector>

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
  void load_audio();
  bool read_audio(const std::string& name, std::vector<SZ_BYTE>& audio);
  bool if_playing_;

  std::vector<SZ_BYTE> success_audio_;
  std::vector<SZ_BYTE> fail_audio_;
  std::vector<SZ_BYTE> temp_normal_audio_;
  std::vector<SZ_BYTE> temp_abnormal_audio_;
};

}  // namespace suanzi

#endif
