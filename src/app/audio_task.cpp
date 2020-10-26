#include "audio_task.hpp"

#include <QFile>
#include <QThread>
#include <QTimer>

#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;

AudioTask* AudioTask::get_instance() {
  static AudioTask instance;
  return &instance;
}

bool AudioTask::idle() { return !get_instance()->is_running_; }

AudioTask::AudioTask(QThread* thread, QObject* parent) : is_running_(false) {
  // Load volume
  int volume_percent = 100;
  if (!Config::read_audio_volume(volume_percent)) {
    SZ_LOG_ERROR("Read audio failed");
  }

  SZ_LOG_INFO("Set audio volume {}", volume_percent);
  auto engine = io::Engine::instance();
  engine->audio_set_volume(volume_percent);

  // Load audio resources
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
  std::string lang = Config::get_user_lang();
  std::string prefix = ":asserts/" + lang;
  SZ_LOG_INFO("Load audio for lang={}", lang);

  read_audio(prefix + "/recognition_succeed.aac", success_audio_);
  read_audio(prefix + "/recognition_failed.aac", fail_audio_);

  read_audio(prefix + "/temperature_normal.aac", temperature_normal_audio_);
  read_audio(prefix + "/temperature_abnormal.aac", temperature_abnormal_audio_);

  read_audio(prefix + "/get_closer.aac", warn_distance_audio_);
  read_audio(prefix + "/take_off_mask.aac", warn_mask_audio_);

  if (lang == "en") {
    success_audio_.duration = 1500;
    fail_audio_.duration = 1500;

    temperature_normal_audio_.duration = 1000;
    temperature_abnormal_audio_.duration = 1500;

    warn_distance_audio_.duration = 1500;
    warn_mask_audio_.duration = 2500;
  } else if (lang == "zh-CN") {
    success_audio_.duration = 1000;
    fail_audio_.duration = 1000;

    temperature_normal_audio_.duration = 2000;
    temperature_abnormal_audio_.duration = 2000;

    warn_distance_audio_.duration = 2000;
    warn_mask_audio_.duration = 3000;
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

void AudioTask::rx_report_person(PersonData person) {
  auto user = Config::get_user();
  if (!user.enable_audio) return;

  is_running_ = true;

  if (!person.is_status_normal())
    play_audio(fail_audio_);
  else
    play_audio(success_audio_);

  if (user.disabled_temperature)
    QThread::msleep(1000);
  else {
    if (!person.is_temperature_normal())
      play_audio(temperature_abnormal_audio_);
    else
      play_audio(temperature_normal_audio_);
  }

  is_running_ = false;
}

void AudioTask::rx_warn_mask() {
  auto user = Config::get_user();
  if (!user.enable_audio) return;

  is_running_ = true;
  play_audio(warn_mask_audio_);
  is_running_ = false;
}

void AudioTask::rx_warn_distance() {
  auto user = Config::get_user();
  if (!user.enable_audio || user.disabled_temperature) return;

  is_running_ = true;
  play_audio(warn_distance_audio_);
  is_running_ = false;
}
