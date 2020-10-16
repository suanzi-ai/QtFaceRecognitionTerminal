#include "audio_task.hpp"

#include <QFile>
#include <QThread>
#include <QTimer>

#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;

AudioTask::AudioTask(QThread* thread, QObject* parent) {
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

  is_reporting_ = false;
}

AudioTask::~AudioTask() {}

void AudioTask::load_audio() {
  std::string lang = Config::get_user_lang();

  SZ_LOG_INFO("Load audio for lang={}", lang);

  std::string prefix = ":asserts/" + lang;
  read_audio(prefix + "/recognition_succeed.aac", success_audio_);
  read_audio(prefix + "/get_closer.aac", warn_audio_);
  read_audio(prefix + "/recognition_failed.aac", fail_audio_);
  read_audio(prefix + "/temperature_normal.aac", temp_normal_audio_);
  read_audio(prefix + "/temperature_abnormal.aac", temp_abnormal_audio_);

  if (lang == "en") {
    success_audio_.duration = 1500;
    warn_audio_.duration = 1500;
    fail_audio_.duration = 1500;
    temp_normal_audio_.duration = 1000;
    temp_abnormal_audio_.duration = 1500;
  } else if (lang == "zh-CN") {
    success_audio_.duration = 1000;
    warn_audio_.duration = 2000;
    fail_audio_.duration = 1000;
    temp_normal_audio_.duration = 2000;
    temp_abnormal_audio_.duration = 2000;
  }
}

bool AudioTask::read_audio(const std::string& name, Audio& audio) {
  QFile audio_file(name.c_str());
  if (!audio_file.open(QIODevice::ReadOnly)) {
    SZ_LOG_ERROR("Open {} failed", name);
    return false;
  }
  auto data = audio_file.readAll();
  audio.data.assign(data.begin(), data.end());
  return true;
}

void AudioTask::play_audio(Audio& audio) {
  io::Engine::instance()->audio_play(audio.data);
  QThread::msleep(audio.duration);
}

void AudioTask::rx_report(PersonData person) {
  auto user = Config::get_user();
  if (!user.enable_audio) return;

  is_reporting_ = true;

  if (!person.is_status_normal())
    play_audio(fail_audio_);
  else
    play_audio(success_audio_);

  if (user.disabled_temperature)
    QThread::msleep(1000);
  else {
    if (!person.is_temperature_normal())
      play_audio(temp_abnormal_audio_);
    else
      play_audio(temp_normal_audio_);
  }

  emit tx_finish();
  is_reporting_ = false;
}

void AudioTask::rx_warn() {
  auto user = Config::get_user();
  if (user.enable_audio && !user.disabled_temperature && !is_reporting_) {
    play_audio(warn_audio_);
  }

  emit tx_warn_finish();
}
