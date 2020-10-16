#include "record_task.hpp"

#include <QThread>
#include <chrono>
#include <quface/logger.hpp>
#include <string>

#include "config.hpp"

using namespace suanzi;

RecordTask::RecordTask(PersonService::ptr person_service, FaceDatabasePtr db,
                       QThread *thread, QObject *parent)
    : person_service_(person_service), db_(db) {
  // Create db for unknown faces
  unknown_database_ = std::make_shared<FaceDatabase>("_UNKNOWN_DB_");
  reset_counter_ = 0;
  body_temperature_ = 0;
  audio_finished_ = true;

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

  if (!input->has_live && !input->has_person_info &&
      ++reset_counter_ > Config::get_extract().max_lost_age) {
    rx_reset();
    emit tx_finish();
    return;
  }

  // reset if new person appear
  if (input->has_person_info && !if_new(input->person_feature)) rx_reset();

  int live_size = Config::get_liveness().history_size;
  int person_size = Config::get_extract().history_size;

  // receive liveness data
  if (input->has_live) live_history_.push_back(input->is_live);

  // do sequence anti_spoof
  bool is_live = sequence_antispoof(live_history_);

  if (live_history_.size() > live_size)
    live_history_.erase(live_history_.begin() + live_size, live_history_.end());

  if (is_live || live_history_.size() == live_size) emit tx_nir_finish(true);

  // receive recognize data
  if (input->has_person_info) person_history_.push_back(input->person_info);

  if (person_history_.size() > person_size)
    person_history_.erase(person_history_.begin() + person_size,
                          person_history_.end());

  if (person_history_.size() == person_size) emit tx_bgr_finish(true);

  // do sequence inference
  if ((is_live || live_history_.size() == live_size) &&
      person_history_.size() == person_size) {
    PersonData person;
    int width = input->img_bgr_large->width;
    int height = input->img_bgr_large->height;
    person.bgr_face_snapshot.create(height, width, CV_8UC3);
    memcpy(person.bgr_face_snapshot.data, input->img_bgr_large->pData,
           width * height * 3 / 2);

    width = input->img_nir_large->width;
    height = input->img_nir_large->height;
    person.nir_face_snapshot.create(height, width, CV_8UC3);
    memcpy(person.nir_face_snapshot.data, input->img_nir_large->pData,
           width * height * 3 / 2);

    // query person info
    SZ_UINT32 face_id;
    if (!sequence_query(person_history_, face_id, person.score) ||
        SZ_RETCODE_OK != person_service_->get_person(face_id, person)) {
      person.id = 0;
      person.score = 0;
      person.name = tr("访客").toStdString();
      person.face_path = ":asserts/avatar_unknown.jpg";
      person.status = person_service_->get_status(PersonStatus::Stranger);
    }

    // decide fake or live
    if (!is_live) {
      if (Config::get_user().liveness_policy == "alarm") {
        person.name = tr("活体失败").toStdString();
      } else {  // stranger
        person.name = tr("访客").toStdString();
      }
      person.id = 0;
      person.score = 0;
      person.face_path = ":asserts/avatar_unknown.jpg";
      person.status = person_service_->get_status(PersonStatus::Fake);
    }

    if (person.is_status_blacklist()) {
      if (Config::get_user().blacklist_policy == "alarm") {
        person.name = tr("黑名单").toStdString();
      } else {  // stranger
        person.name = tr("访客").toStdString();
      }
      person.id = 0;
      person.score = 0;
      person.face_path = ":asserts/avatar_unknown.jpg";
    }

    // decide duplicate
    bool duplicated;
    person.temperature = body_temperature_;
    if (!person.is_status_normal()) {
      duplicated = if_duplicated(input->person_feature, person.temperature);
    } else {
      duplicated = if_duplicated(face_id, person.temperature);

      // Update face feature
      if (person.score < 0.9) db_->add(face_id, input->person_feature, 0.1);
    }

    // output
    rx_reset();
    if (!Config::get_user().disabled_temperature)
      SZ_LOG_INFO(
          "Record: id={}, staff={}, score={:.2f}, status={}, temperature={}",
          person.id, person.number, person.score, person.status,
          person.temperature);
    else
      SZ_LOG_INFO("Record: id={}, staff={}, score={:.2f}, status={}", person.id,
                  person.number, person.score, person.status);

    if (is_live) emit tx_display(person, duplicated);
    if (audio_finished_) {
      audio_finished_ = false;
      emit tx_audio(person);
    }
  }

  emit tx_finish();
}

void RecordTask::rx_reset() {
  person_history_.clear();
  live_history_.clear();
  reset_counter_ = 0;

  emit tx_nir_finish(false);
  emit tx_bgr_finish(false);
}

void RecordTask::rx_audio_finish() { audio_finished_ = true; }

bool RecordTask::if_new(const FaceFeature &feature) {
  bool ret;
  if (person_history_.size() == 0) {
    ret = true;
  } else {
    float score = 0.0;

#if __ARM_NEON
    assert(SZ_FEATURE_NUM % 16 == 0);
    int dim = SZ_FEATURE_NUM;

    const float *com_feat = feature.value;
    const float *q_feat = last_feature_.value;
    float32x4_t out = vmovq_n_f32(0.0);
    float32x4_t f1, f2;
    float outTmp[4];
    for (int k = 0; k < dim; k += 16) {
      f1 = vld1q_f32(com_feat + k);
      f2 = vld1q_f32(q_feat + k);
      out = vmlaq_f32(out, f1, f2);

      f1 = vld1q_f32(com_feat + k + 4);
      f2 = vld1q_f32(q_feat + k + 4);
      out = vmlaq_f32(out, f1, f2);

      f1 = vld1q_f32(com_feat + k + 8);
      f2 = vld1q_f32(q_feat + k + 8);
      out = vmlaq_f32(out, f1, f2);

      f1 = vld1q_f32(com_feat + k + 12);
      f2 = vld1q_f32(q_feat + k + 12);
      out = vmlaq_f32(out, f1, f2);
    }
    vst1q_f32(outTmp, out);

    score = outTmp[0] + outTmp[1] + outTmp[2] + outTmp[3];
#else
    for (int k = 0; k < SZ_FEATURE_NUM; k++)
      score += feature.value[k] * last_feature_.value[k];
#endif

    ret = score / 2 + 0.5f > 0.9;
  }
  memcpy(last_feature_.value, feature.value, SZ_FEATURE_NUM * sizeof(SZ_FLOAT));

  return ret;
}

bool RecordTask::sequence_query(const std::vector<QueryResult> &history,
                                SZ_UINT32 &face_id, SZ_FLOAT &score) {
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

  SZ_LOG_DEBUG("count={}/{}, max={:.2f}/{:.2f}, sum={:.2f}/{:.2f}", max_count,
               cfg.history_size, max_person_score, cfg.min_recognize_score,
               person_accumulate_score[max_person_id],
               cfg.min_accumulate_score);

  if (max_count >= cfg.min_recognize_count &&
      max_person_accumulate_score >= cfg.min_accumulate_score &&
      max_person_score >= cfg.min_recognize_score) {
    face_id = max_person_id;
    score = max_person_score;
    return true;
  }
  if (max_count == cfg.history_size &&
      max_person_accumulate_score >= cfg.min_accumulate_score) {
    face_id = max_person_id;
    score = max_person_score;
    return true;
  } else {
    return false;
  }
}

bool RecordTask::sequence_antispoof(const std::vector<bool> &history) {
  if (history.size() == 0) return false;

  int count = 0;
  for (auto is_live : history) {
    if (is_live) count++;
  }

  bool ret = count >= Config::get_liveness().min_alive_count;
  if (ret || history.size() == Config::get_liveness().history_size)
    SZ_LOG_DEBUG("live={}/{}/{}", count, Config::get_liveness().min_alive_count,
                 history.size());

  return ret;
}

bool RecordTask::if_duplicated(const SZ_UINT32 &face_id, float &temperature) {
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
      known_temperature_[face_id] = temperature;
      return false;
    } else {
      if (std::abs(temperature - known_temperature_[face_id]) < 1)
        temperature = known_temperature_[face_id];
      else
        known_temperature_[face_id] = temperature;

      return true;
    }
  } else {
    query_clock_[face_id] = current_query_clock;
    known_temperature_[face_id] = temperature;
    return false;
  }
}

bool RecordTask::if_duplicated(const FaceFeature &feature, float &temperature) {
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
      unknown_temperature_[face_id] = temperature;
      return false;
    } else {
      if (std::abs(temperature - unknown_temperature_[face_id]) < 1)
        temperature = unknown_temperature_[face_id];
      else
        unknown_temperature_[face_id] = temperature;

      return true;
    }
  } else {
    if (face_id == -1) face_id = (db_size % 100) + 1;

    unknown_database_->add(face_id, feature);
    unknown_query_clock_[face_id] = current_query_clock;
    unknown_temperature_[face_id] = temperature;
    return false;
  }
}

void RecordTask::rx_temperature(float body_temperature) {
  body_temperature_ = body_temperature;
}
