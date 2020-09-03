#include "audio_task.hpp"

#include <QThread>
#include <QTimer>

#include <quface-io/engine.hpp>

using namespace suanzi;

AudioTask::AudioTask(QThread *thread, QObject *parent) : if_playing_(false) {
  auto engine = io::Engine::instance();
  engine->audio_set_volume(6);

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

void AudioTask::rx_display(PersonData person, bool if_duplicated) {
  auto engine = io::Engine::instance();
  if (!if_playing_) {
    if_playing_ = true;

    if (person.status != PersonService::get_status(PersonStatus::Normal))
      engine->audio_play(":asserts/fail.aac");
    else
      engine->audio_play(":asserts/success.aac");

    QTimer::singleShot(1000, this, SLOT(rx_reset()));
  }
}

void AudioTask::rx_reset() { if_playing_ = false; }
