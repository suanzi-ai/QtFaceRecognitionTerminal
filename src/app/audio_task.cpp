#include "audio_task.hpp"

#include <QFile>
#include <QThread>
#include <QTimer>
#include <quface-io/engine.hpp>

using namespace suanzi;

AudioTask::AudioTask(QThread* thread, QObject* parent) : if_playing_(false) {
  auto engine = io::Engine::instance();
  engine->audio_set_volume(6);

  read_audio(":asserts/success.aac", success_audio_);
  read_audio(":asserts/fail.aac", fail_audio_);

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

AudioTask::~AudioTask() {}

bool AudioTask::read_audio(const std::string& name,
                           std::vector<SZ_BYTE>& audio) {
  QFile audio_file(name.c_str());
  if (!audio_file.open(QIODevice::ReadOnly)) {
    SZ_LOG_ERROR("Open {} failed", name);
    return false;
  }
  auto data = audio_file.readAll();
  audio.assign(data.begin(), data.end());
  return true;
}

void AudioTask::rx_display(PersonData person, bool if_duplicated) {
  auto engine = io::Engine::instance();
  if (!if_playing_) {
    if_playing_ = true;

    if (person.status != PersonService::get_status(PersonStatus::Normal))
      engine->audio_play(fail_audio_);
    else
      engine->audio_play(success_audio_);

    QTimer::singleShot(1000, this, SLOT(rx_reset()));
  }
}

void AudioTask::rx_reset() { if_playing_ = false; }
