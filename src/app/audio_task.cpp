#include "audio_task.hpp"

#include <QFile>
#include <QThread>
#include <QTimer>
#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;

AudioTask::AudioTask(QThread* thread, QObject* parent) : if_playing_(false) {
  auto engine = io::Engine::instance();
  engine->audio_set_volume(100);

  load_audio();

  Config::get_instance()->appendListener("reload", [&]() { load_audio(); });

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

void AudioTask::load_audio() {
  std::string lang = Config::get_user().lang;
  std::string prefix = ":asserts/" + lang;

  SZ_LOG_INFO("Load audio for lang={}", lang);

  read_audio(prefix + "/recognition_succeed.aac", success_audio_);
  read_audio(prefix + "/recognition_failed.aac", fail_audio_);
  read_audio(prefix + "/temperature_normal.aac", temp_normal_audio_);
  read_audio(prefix + "/temperature_abnormal.aac", temp_abnormal_audio_);
}

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
  auto user = Config::get_user();
  if (!user.enable_audio) return;

  auto engine = io::Engine::instance();
  if (!if_playing_) {
    if_playing_ = true;

    if (!person.is_status_normal())
      engine->audio_play(fail_audio_);
    else
      engine->audio_play(success_audio_);

    if (!user.disabled_temperature) {
      if (!person.is_temperature_normal())
        engine->audio_play(temp_abnormal_audio_);
      else
        engine->audio_play(temp_normal_audio_);
    }

    if (if_duplicated) QTimer::singleShot(4000, this, SLOT(rx_reset()));
    else rx_reset();
  }
}

void AudioTask::rx_reset() { if_playing_ = false; }
