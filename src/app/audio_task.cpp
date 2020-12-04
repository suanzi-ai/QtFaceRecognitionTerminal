#include "audio_task.hpp"

#include <QFile>
#include <QThread>
#include <QTimer>

#include <quface-io/engine.hpp>

#include "config.hpp"
#include "gpio_task.hpp"

using namespace suanzi;

AudioTask* AudioTask::get_instance() {
  static AudioTask instance;
  return &instance;
}

bool AudioTask::idle() { return !get_instance()->is_running_; }

SZ_UINT16 AudioTask::duration(PersonData person) {
  auto user = Config::get_user();
  auto instance = get_instance();
  SZ_UINT16 total_duration = 0;
  if (!user.enable_audio) return total_duration;

  if (user.enable_record_audio && !person.is_status_normal())
    total_duration += instance->fail_audio_.duration;

  if (user.enable_temperature) {
    if (user.enable_mask_audio && !person.has_mask)
      total_duration += instance->warn_mask_audio_.duration;

    if (user.enable_temperature_audio) {
      if (!person.is_temperature_normal())
        total_duration += instance->temperature_abnormal_audio_.duration;
      else
        total_duration += instance->temperature_normal_audio_.duration;
    }

    if ((user.enable_record_audio || user.enable_mask_audio) &&
        user.enable_pass_audio) {
      if (person.is_status_normal() && person.is_temperature_normal() &&
          person.has_mask)
        total_duration += instance->pass_audio_.duration;
    }
  } else if (user.enable_record_audio && user.enable_pass_audio)
    if (person.is_status_normal())
      total_duration += instance->pass_audio_.duration;

  // SZ_LOG_INFO("total_duration={}", total_duration / 1000);
  return total_duration / 1000 + 1;
}

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

void AudioTask::beep() {
  auto user = Config::get_user();
  if (!user.enable_audio) return;

  is_running_ = true;
  play_audio(beep_audio_);
  is_running_ = false;
}

void AudioTask::load_audio() {
  std::string lang = Config::get_user_lang();
  std::string prefix = ":asserts/" + lang;
  SZ_LOG_INFO("Load audio for lang={}", lang);

  read_audio(prefix + "/recognition_succeed.aac", success_audio_);
  read_audio(prefix + "/recognition_failed.aac", fail_audio_);

  read_audio(prefix + "/temperature_normal.aac", temperature_normal_audio_);
  read_audio(prefix + "/temperature_abnormal.aac", temperature_abnormal_audio_);

  read_audio(prefix + "/get_closer.aac", warn_distance_audio_);
  read_audio(prefix + "/take_on_mask.aac", warn_mask_audio_);

  read_audio(prefix + "/pass.aac", pass_audio_);

  if (lang == "en") {
    success_audio_.duration = 1500;
    fail_audio_.duration = 1500;

    temperature_normal_audio_.duration = 1000;
    temperature_abnormal_audio_.duration = 1500;

    warn_distance_audio_.duration = 1500;
    warn_mask_audio_.duration = 2000;

    pass_audio_.duration = 1000;
  } else if (lang == "zh-CN") {
    success_audio_.duration = 1000;
    fail_audio_.duration = 1000;

    temperature_normal_audio_.duration = 2000;
    temperature_abnormal_audio_.duration = 2000;

    warn_distance_audio_.duration = 2000;
    warn_mask_audio_.duration = 1000;

    pass_audio_.duration = 1000;
  } else if (lang == "jp") {
    success_audio_.duration = 0;
    fail_audio_.duration = 1500;

    temperature_normal_audio_.duration = 1500;
    temperature_abnormal_audio_.duration = 1500;

    warn_distance_audio_.duration = 3000;
    warn_mask_audio_.duration = 1500;

    pass_audio_.duration = 2500;
  }

  read_audio(":asserts/beep.aac", beep_audio_);
  beep_audio_.duration = 250;
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
  if (audio.duration > 0) {
    io::Engine::instance()->audio_play(audio.data);
    QThread::msleep(audio.duration);
  }
}

void AudioTask::rx_report(PersonData person, bool audio_duplicated,
                          bool record_duplicated) {
  auto user = Config::get_user();
  if (!user.enable_audio || audio_duplicated) return;

  is_running_ = true;

  if (user.enable_record_audio && !person.is_status_normal())
    play_audio(fail_audio_);

  if (user.enable_mask_audio && !person.has_mask) play_audio(warn_mask_audio_);

  if (user.enable_temperature) {
    if (user.enable_temperature_audio) {
      if (!person.is_temperature_normal())
        play_audio(temperature_abnormal_audio_);
      else
        play_audio(temperature_normal_audio_);
    }
  }

  if (GPIOTask::validate(person)) play_audio(pass_audio_);

  is_running_ = false;
}

void AudioTask::rx_warn_distance() {
  auto user = Config::get_user();
  if (!user.enable_audio || !user.enable_distance_audio ||
      !user.enable_temperature)
    return;

  is_running_ = true;
  play_audio(warn_distance_audio_);
  is_running_ = false;
}
