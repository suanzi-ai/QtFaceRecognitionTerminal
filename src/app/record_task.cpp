#include "record_task.hpp"

#include <chrono>
#include <string>

#include <QThread>

#include "config.hpp"
#include "ive.h"
#include "logger.hpp"

using namespace suanzi;

RecordTask::RecordTask(PersonService::ptr person_service, QThread *thread,
                       QObject *parent)
    : person_service_(person_service) {
  // Create db for unknown faces
  unknown_database_ = std::make_shared<FaceDatabase>("_UNKNOWN_DB_");

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

RecordTask::~RecordTask() {
  if (unknown_database_) unknown_database_.reset();

  person_history_.clear();
  live_history_.clear();
  query_clock_.clear();
  unknown_query_clock_.clear();
}

void RecordTask::rx_frame(PingPangBuffer<RecognizeData> *buffer) {
  buffer->switch_buffer();
  RecognizeData *input = buffer->get_pang();

  live_history_.push_back(input->is_live);
  if (live_history_.size() > Config::get_liveness().history_size)
    live_history_.erase(live_history_.begin());

  person_history_.push_back(input->person_info);
  if (person_history_.size() > Config::get_extract().history_size)
    person_history_.erase(person_history_.begin());

  if (live_history_.size() == Config::get_liveness().history_size &&
      person_history_.size() == Config::get_extract().history_size) {
    PersonData person;

    static MmzImage *snapshot =
        new MmzImage(224, 320, SZ_IMAGETYPE_BGR_PACKAGE);

    int width = input->img_bgr_small->width;
    int height = input->img_bgr_small->height;
    snapshot->setSize(width, height);
    if (Ive::getInstance()->yuv2RgbPacked(snapshot, input->img_bgr_small,
                                          true)) {
      cv::Mat(height, width, CV_8UC3, snapshot->pData)
          .copyTo(person.face_snapshot);
    } else {
      SZ_LOG_ERROR("upload snapshot failed");
    }

    // query person info
    SZ_UINT32 face_id;
    if (!sequence_query(person_history_, face_id) ||
        SZ_RETCODE_OK != person_service_->get_person(face_id, person)) {
      person.id = 0;
      person.name = "访客";
      person.face_path = ":asserts/avatar_unknown.jpg";
      person.status = person_service_->get_status(PersonStatus::Stranger);
    }

    // decide fake or live
    if (!sequence_antispoof(live_history_)) {
      person.id = 0;
      person.name = "异常";
      person.face_path = ":asserts/avatar_unknown.jpg";
      person.status = person_service_->get_status(PersonStatus::Fake);
    }

    if (person.status == person_service_->get_status(PersonStatus::Blacklist)) {
      person.id = 0;
      person.name = "异常";
      person.face_path = ":asserts/avatar_unknown.jpg";
    }

    // decide duplicate
    bool duplicated_;
    if (person.status != person_service_->get_status(PersonStatus::Normal)) {
      duplicated_ = if_duplicated(input->person_feature);
    } else
      duplicated_ = if_duplicated(face_id);

    // output
    rx_reset();
    SZ_LOG_INFO("record: person_number={}, status={}", person.number,
                person.status);
    emit tx_display(person, duplicated_);
  }

  emit tx_finish();
}

void RecordTask::rx_reset() {
  person_history_.clear();
  live_history_.clear();
}

bool RecordTask::sequence_query(const std::vector<QueryResult> &history,
                                SZ_UINT32 &face_id) {
  auto cfg = Config::get_extract();
  std::map<SZ_UINT32, int> person_counts;
  std::map<SZ_UINT32, float> person_accumulate_score;
  std::map<SZ_UINT32, float> person_max_score;

  // initialize map
  for (auto &person : history) {
    person_counts[person.face_id] = 0;
    person_accumulate_score[person.face_id] = 0.f;
    person_max_score[person.face_id] = 0.f;
  }

  // accumulate id and score
  for (auto &person : history) {
    person_counts[person.face_id] += 1;
    person_accumulate_score[person.face_id] += person.score;
    person_max_score[person.face_id] =
        std::max(person.score, person_max_score[person.face_id]);
  }

  // person id with max counts
  auto max_person = std::max_element(
      person_counts.begin(), person_counts.end(),
      [](const std::pair<int, int> &i, const std::pair<int, int> &j) {
        return i.second < j.second;
      });

  SZ_UINT32 max_person_id = max_person->first;
  int max_count = max_person->second;
  float max_person_accumulate_score = person_accumulate_score[max_person_id];
  float max_person_score = person_max_score[max_person_id];

  SZ_LOG_DEBUG("id={}, count={}, max_score={}, accumulate_score={}",
               max_person_id, max_count, max_person_score,
               person_accumulate_score[max_person_id]);
  if (max_count >= cfg.min_recognize_count &&
      max_person_accumulate_score >= cfg.min_accumulate_score &&
      max_person_score >= cfg.min_recognize_score) {
    face_id = max_person_id;
    return true;
  } else {
    return false;
  }
}

bool RecordTask::sequence_antispoof(const std::vector<bool> &history) {
  int count = 0;
  for (auto is_live : history) {
    if (is_live) count++;
  }

  SZ_LOG_DEBUG("live count={}", count);
  return count >= Config::get_liveness().min_alive_count;
}

bool RecordTask::if_duplicated(const SZ_UINT32 &face_id) {
  auto cfg = Config::get_user();
  bool ret = false;

  auto current_query_clock = std::chrono::steady_clock::now();
  if (query_clock_.find(face_id) != query_clock_.end()) {
    auto last_query_clock = query_clock_[face_id];
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                        current_query_clock - last_query_clock)
                        .count();

    if (duration > cfg.duplication_interval) {
      query_clock_[face_id] = current_query_clock;
      return false;
    } else
      return true;
  } else {
    query_clock_[face_id] = current_query_clock;
    return false;
  }
}

bool RecordTask::if_duplicated(const FaceFeature &feature) {
  auto cfg = Config::get_user();
  bool ret = false;

  auto current_query_clock = std::chrono::steady_clock::now();

  SZ_UINT32 db_size;
  unknown_database_->size(db_size);

  int face_id = -1;
  if (db_size != 0) {
    static std::vector<QueryResult> results;
    results.clear();

    SZ_RETCODE ret = unknown_database_->query(feature, 1, results);
    if (SZ_RETCODE_OK == ret && results[0].score >= 0.8) {
      face_id = results[0].face_id;
      unknown_database_->add(face_id, feature);
    }
  }

  if (face_id > 0 &&
      unknown_query_clock_.find(face_id) != unknown_query_clock_.end()) {
    auto last_query_clock = unknown_query_clock_[face_id];
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(
                        current_query_clock - last_query_clock)
                        .count();

    if (duration > cfg.duplication_interval) {
      unknown_query_clock_[face_id] = current_query_clock;
      return false;
    } else
      return true;
  } else {
    if (face_id == -1) face_id = (db_size % 100) + 1;

    unknown_database_->add(face_id, feature);
    unknown_query_clock_[face_id] = current_query_clock;
    return false;
  }
}