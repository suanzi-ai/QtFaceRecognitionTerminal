#include "record_task.hpp"

#include <chrono>
#include <string>

#include <QThread>

#include <opencv2/opencv.hpp>

#include <quface-io/ive.hpp>
#include <quface/logger.hpp>

#include "audio_task.hpp"
#include "config.hpp"

#define CONTAIN_KEY(dict, key) ((dict).find((key)) != (dict).end())
#define SECONDS_DIFF(t1, t2) \
  (std::chrono::duration_cast<std::chrono::seconds>((t1) - (t2)).count())
#define GOOD_TEMPERATURE(t) (Config::get_user().temperature_max >= (t))

using namespace suanzi;
using namespace suanzi::io;

RecordTask *RecordTask::get_instance() {
  static RecordTask instance;
  return &instance;
}

bool RecordTask::idle() { return !get_instance()->is_running_; }

bool RecordTask::card_readed() { return get_instance()->has_card_no_; };

void RecordTask::clear_temperature() {
  auto task = get_instance();

  task->known_temperature_.clear();
  task->unknown_temperature_.clear();
}

RecordTask::RecordTask(QThread *thread, QObject *parent)
    : is_running_(false),
      duplicated_counter_(0),
      latest_temperature_(0),
      has_unhandle_person_(false),
      has_card_no_(false),
      is_enabled_(true) {
  person_service_ = PersonService::get_instance();
  face_database_ = std::make_shared<FaceDatabase>(Config::get_quface().db_name);

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
  temperature_history_.clear();
}

void RecordTask::rx_frame(PingPangBuffer<RecognizeData> *buffer) {
  if (is_running_) return;

  is_running_ = true;

  buffer->switch_buffer();
  RecognizeData *input = buffer->get_pang();

  bool bgr_finished = false, ir_finished = false;
  bool has_mask;
  bool update_record = false;

  if (input->has_person_info) {
    // reset if new person appear
    if (if_fresh(input->person_feature)) {
      reset_recognize();
      reset_temperature();

      has_unhandle_person_ = false;
      update_record = true;
    }

    // add person info
    mask_history_.push_back(input->has_mask);
    person_history_.push_back(input->person_info);

    // do sequence mask detection
    bgr_finished = sequence_mask(mask_history_, has_mask);
  }

  bool is_live = false;
  if (input->has_live) {
    // add antispoofing data
    live_history_.push_back(input->is_live);

    // do sequence antispoofing
    ir_finished = sequence_antispoof(live_history_, is_live);
  }

  if (has_card_no_) {
    PersonData person;
    update_person_info(input, card_no_, person);
    person.has_mask = true;
    emit tx_display(person, false, false);

    rx_reset();
    QThread::msleep(AudioTask::duration(person) * 1000);

    has_card_no_ = false;

  } else if (is_enabled_ && bgr_finished && ir_finished) {
    if (is_live) {
      SZ_UINT32 face_id;
      PersonData person;
      if (sequence_query(person_history_, mask_history_, has_mask, face_id,
                         person.score)) {
        if (has_mask && person.score < 0.85)
          face_database_->add(face_id, input->person_feature, 0.1);
        if (!has_mask && person.score < 0.9)
          face_database_->add(face_id, input->person_feature, 0.1);
      }
      person.has_mask = has_mask;

      update_person_info(input, face_id, person);

      auto cfg = Config::get_user();
      if (duplicated_counter_ < cfg.duplication_limit) {
        int duration;
        bool duplicated =
            if_duplicated(face_id, latest_feature_, duration, person);

        if (Config::has_temperature_device()) {
          if (!duplicated) latest_temperature_ = 0;
          update_record |= update_person_temperature(face_id, duration, person);
          if (person.temperature > 0) {
            has_unhandle_person_ = false;
            if (!duplicated) duplicated_counter_++;
            emit tx_display(person, duplicated, !update_record);
          } else if (latest_temperature_ == 0) {
            duplicated_id_ = face_id;
            duplicated_duration_ = duration;
            latest_person_ = person;
            has_unhandle_person_ = true;
          }
        } else {
          update_record = !duplicated;
          if (!duplicated) duplicated_counter_++;
          emit tx_display(person, duplicated, !update_record);
        }
      }
    }
    reset_recognize();
  }

  is_running_ = false;
}

bool RecordTask::if_fresh(const FaceFeature &feature) {
  float score = 0.0;

#if __ARM_NEON
  assert(SZ_FEATURE_NUM % 16 == 0);
  int dim = SZ_FEATURE_NUM;

  const float *com_feat = feature.value;
  const float *q_feat = latest_feature_.value;
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
    score += feature.value[k] * latest_feature_.value[k];
#endif

  memcpy(latest_feature_.value, feature.value,
         SZ_FEATURE_NUM * sizeof(SZ_FLOAT));

  return score / 2 + 0.5f < 0.75;
}

void RecordTask::reset_recognize() {
  mask_history_.clear();
  person_history_.clear();
  live_history_.clear();

  emit tx_nir_finish(false);
  emit tx_bgr_finish(false);
}

void RecordTask::reset_temperature() {
  temperature_history_.clear();
  latest_temperature_ = 0;
}

bool RecordTask::sequence_query(const std::vector<QueryResult> &person_history,
                                const std::vector<bool> &mask_history,
                                const bool has_mask, SZ_UINT32 &face_id,
                                SZ_FLOAT &score) {
  // initialize map
  std::map<SZ_UINT32, int> person_counts;
  std::map<SZ_UINT32, float> person_accumulate_score;
  std::map<SZ_UINT32, float> person_max_score;
  for (auto &person : person_history) {
    person_counts[person.face_id] = 0;
    person_accumulate_score[person.face_id] = 0.f;
    person_max_score[person.face_id] = 0.f;
  }

  // accumulate id and score
  int max_count = 0;
  float accumulate_score;

  auto pit = person_history.rbegin();
  auto mit = mask_history.rbegin();
  while (pit != person_history.rend() && mit != mask_history.rend()) {
    if (*mit == has_mask) {
      person_counts[pit->face_id] += 1;
      person_accumulate_score[pit->face_id] += pit->score;
      person_max_score[pit->face_id] =
          std::max(pit->score, person_max_score[pit->face_id]);

      if (person_counts[pit->face_id] > max_count) {
        max_count = person_counts[pit->face_id];
        face_id = pit->face_id;
        score = person_max_score[face_id];
        accumulate_score = person_accumulate_score[face_id];
      }
    }
    pit++;
    mit++;
  }

  auto cfg = Config::get_extract();
  SZ_LOG_DEBUG("count={}/{}, max={:.2f}/{:.2f}, sum={:.2f}/{:.2f}", max_count,
               cfg.history_size, score, cfg.min_recognize_score,
               accumulate_score, cfg.min_accumulate_score);

  if (max_count >= cfg.min_recognize_count &&
      score >= cfg.min_recognize_score &&
      accumulate_score >= cfg.min_accumulate_score)
    return true;

  if (max_count == cfg.history_size &&
      accumulate_score >= cfg.min_accumulate_score)
    return true;

  score = -1;
  return false;
}

bool RecordTask::sequence_antispoof(const std::vector<bool> &history,
                                    bool &is_live) {
  int min_count = Config::get_liveness().min_alive_count;
  int max_count = Config::get_liveness().history_size;
  if (history.size() < min_count) return false;

  int live_count = 0, count = 0;
  auto it = history.rbegin();
  while (it != history.rend()) {
    if (*it && ++live_count >= min_count) {
      is_live = true;
      return true;
    }
    if (++count >= max_count) {
      is_live = false;
      return true;
    }
    it++;
  }

  return false;
}

bool RecordTask::sequence_mask(const std::vector<bool> &history,
                               bool &has_mask) {
  int max_person = Config::get_extract().history_size;
  if (history.size() < max_person) return false;

  int mask = 0, no_mask = 0;
  auto it = history.rbegin();
  while (it != history.rend()) {
    if (*it && ++mask >= max_person) {
      has_mask = true;
      return true;
    }
    if (!(*it) && ++no_mask >= max_person) {
      has_mask = false;
      return true;
    }
    it++;
  }

  return false;
}

bool RecordTask::sequence_temperature(SZ_UINT32 face_id, int duration,
                                      std::map<SZ_UINT32, float> &history,
                                      float &temperature) {
  SZ_LOG_INFO("id={}, duration={}, temperature={:.2f}", face_id, duration,
              temperature);

  const float MAX_TEMPERATURE = Config::get_user().temperature_max;
  const float MIN_TEMPERATURE = 36.3;
  const float RANDOM_TEMPERATURE = MIN_TEMPERATURE + rand() % 2 / 10.f;

  // Low temperature
  if (temperature < MIN_TEMPERATURE) {
    if (!CONTAIN_KEY(history, face_id)) {
      history[face_id] = temperature;
      temperature = RANDOM_TEMPERATURE;
      return true;
    } else {
      // High history
      if (history[face_id] >= MAX_TEMPERATURE) {
        history[face_id] = temperature;
        temperature = RANDOM_TEMPERATURE;
        return true;
      }

      // Normal history
      history[face_id] = std::max(temperature, history[face_id]);
      temperature = std::max(RANDOM_TEMPERATURE, history[face_id]);
      return true;
    }
  }

  // High temperature
  if (temperature >= MAX_TEMPERATURE) {
    if (!CONTAIN_KEY(history, face_id)) {
      history[face_id] = temperature;
      return false;
    } else {
      // High history
      if (history[face_id] >= MAX_TEMPERATURE) {
        if (temperature > history[face_id]) {
          history[face_id] = temperature;
          return false;
        } else {
          temperature = history[face_id];
          return true;
        }
      }

      // Normal history
      if (temperature >= MAX_TEMPERATURE + 0.5f) {
        history[face_id] = temperature;
        return false;
      } else {
        temperature = history[face_id];
        return true;
      }
    }
  }

  // Normal temperature
  if (!CONTAIN_KEY(history, face_id)) {
    history[face_id] = temperature;
    return false;
  } else {
    // High history
    if (history[face_id] >= MAX_TEMPERATURE) {
      history[face_id] = temperature;
      return false;
    }

    // Normal history
    if ((temperature > history[face_id] - 0.2f && duration > 10) ||
        (temperature > history[face_id])) {
      history[face_id] = temperature;
      return false;
    } else {
      temperature = history[face_id];
      return true;
    }
  }
}

bool RecordTask::update_temperature_bias() {
  const float AVE_TEMPERATURE = 36.6;

  float sum = 0, max_temperature = 0, min_temperature = 100;
  int count = 0;
  for (auto &it : known_temperature_) {
    max_temperature = std::max(max_temperature, it.second);
    sum += it.second;
    count += 1;
  }
  for (auto &it : unknown_temperature_) {
    min_temperature = std::min(min_temperature, it.second);
    sum += it.second;
    count += 1;
  }

  if (count >= 5) {
    float bias = Config::get_temperature_bias();
    float diff;
    if (count >= 5)
      diff = AVE_TEMPERATURE -
             (sum - max_temperature - min_temperature) / (count - 2);
    if (diff > 1) diff /= 2;

    for (auto &it : known_temperature_) it.second += diff;
    for (auto &it : unknown_temperature_) it.second += diff;

    Config::set_temperature_finetune(diff);
    if (std::abs(bias - Config::get_temperature_bias()) > 0.5) {
      SZ_LOG_INFO("update bias {:.2f} --> {:.2f}", bias,
                  Config::get_temperature_bias());
      json cfg;
      Config::to_json(cfg);
      Config::get_instance()->save_diff(cfg);
    }

    return true;
  }

  if (count > 10) {
    known_temperature_.clear();
    unknown_temperature_.clear();
  }
  return false;
}

bool RecordTask::update_person_temperature(const SZ_UINT32 &face_id,
                                           int duration, PersonData &person) {
  person.temperature = 0;
  if (temperature_history_.size() < Config::get_temperature().min_size)
    return false;

  for (float temperature : temperature_history_)
    person.temperature = std::max(temperature, person.temperature);
  temperature_history_.clear();

  if (person.temperature > 0 && Config::get_user().enable_temperature) {
    if (person.status == PersonService::get_status(PersonStatus::Stranger))
      sequence_temperature(face_id, duration, unknown_temperature_,
                           person.temperature);
    else
      sequence_temperature(face_id, duration, known_temperature_,
                           person.temperature);
    update_temperature_bias();
  }

  return if_temperature_updated(person.temperature);
}

void RecordTask::update_person_info(RecognizeData *input,
                                    const SZ_UINT32 &face_id,
                                    PersonData &person) {
  PersonStatus status = PersonStatus::Stranger;
  if (person.score > 0 &&
      SZ_RETCODE_OK == person_service_->get_person(face_id, person)) {
    if (person.is_status_normal()) status = PersonStatus::Normal;
    if (person.is_status_blacklist()) status = PersonStatus::Blacklist;
  }

  switch (status) {
    case PersonStatus::Blacklist:
      if (Config::get_user().blacklist_policy == "alarm")
        person.name = tr("黑名单").toStdString();
      else {
        person.name = tr("访客").toStdString();
        person.number = "";
      }
      person.face_path = ":asserts/avatar_unknown.jpg";
    case PersonStatus::Normal:
      break;
    case PersonStatus::Stranger:
      person.name = tr("访客").toStdString();
      person.id = 0;
      person.score = 0;
      person.number = "";
      person.face_path = ":asserts/avatar_unknown.jpg";
      person.status = person_service_->get_status(PersonStatus::Stranger);
      break;
  }

  // record snapshots
  update_person_snapshot(input, person);
}

void RecordTask::update_person_info(RecognizeData *input,
                                    const std::string &card_no,
                                    PersonData &person) {
  PersonStatus status = PersonStatus::Stranger;
  if (SZ_RETCODE_OK == person_service_->get_person(card_no, person)) {
    if (person.is_status_normal()) status = PersonStatus::Normal;
    if (person.is_status_blacklist()) status = PersonStatus::Blacklist;
  }
  person.number = card_no;

  switch (status) {
    case PersonStatus::Blacklist:
      if (Config::get_user().blacklist_policy == "alarm")
        person.name = tr("黑名单").toStdString();
      else {
        person.name = tr("访客").toStdString();
      }
      person.face_path = ":asserts/avatar_unknown.jpg";
    case PersonStatus::Normal:
      break;
    case PersonStatus::Stranger:
      person.name = tr("访客").toStdString();
      person.id = 0;
      person.score = 0;
      person.face_path = ":asserts/avatar_unknown.jpg";
      person.status = person_service_->get_status(PersonStatus::Stranger);
      break;
  }

  // record snapshots
  update_person_snapshot(input, person);
}

void RecordTask::update_person_snapshot(RecognizeData *input,
                                        PersonData &person) {
  MmzImage *bgr, *ir;
  if (Config::get_user().upload_hd_snapshot) {
    bgr = input->img_bgr_large;
    ir = input->img_nir_large;
  } else {
    bgr = input->img_bgr_small;
    ir = input->img_nir_small;
  }

  int width = bgr->width;
  int height = bgr->height;
  person.bgr_snapshot.create(height, width, CV_8UC3);
  memcpy(person.bgr_snapshot.data, bgr->pData, width * height * 3 / 2);

  static MmzImage *snapshot =
      new MmzImage(width, height, SZ_IMAGETYPE_BGR_PACKAGE);
  snapshot->set_size(width, height);
  if (input->bgr_face_detected_ && width < height &&
      Ive::getInstance()->yuv2RgbPacked(snapshot, bgr, true)) {
    int crop_x = input->bgr_detection_.x * width;
    int crop_y = input->bgr_detection_.y * height;
    int crop_w = input->bgr_detection_.width * width;
    int crop_h = input->bgr_detection_.height * height;

    crop_x = std::max(0, crop_x - crop_w / 2);
    crop_y = std::max(0, crop_y - crop_h / 4);
    crop_w = std::min(width - crop_x - 1, crop_w * 2);
    crop_h = std::min(height - crop_y - 1, crop_h * 3 / 2);

    cv::Mat(height, width, CV_8UC3,
            snapshot->pData)({crop_x, crop_y, crop_w, crop_h})
        .copyTo(person.face_snapshot);
  } else
    person.face_snapshot = cv::Mat();

  width = ir->width;
  height = ir->height;
  person.nir_snapshot.create(height, width, CV_8UC3);
  memcpy(person.nir_snapshot.data, ir->pData, width * height * 3 / 2);
}

bool RecordTask::if_duplicated(SZ_UINT32 &face_id, const FaceFeature &feature,
                               int &duration, PersonData &person) {
  bool ret = false;

  auto cfg = Config::get_user();

  auto current_query_clock = std::chrono::steady_clock::now();

  // query known person
  if (person.status != PersonService::get_status(PersonStatus::Stranger)) {
    if (duplicated_counter_ != 0) {
      duration = SECONDS_DIFF(current_query_clock, last_query_clock_);

      if (duration >
          std::max(cfg.duplication_interval, AudioTask::duration(person))) {
        last_query_clock_ = current_query_clock;
      } else
        ret = true;
    } else
      last_query_clock_ = current_query_clock;
  }
  // query unknown person
  else {
    SZ_UINT32 db_size;
    unknown_database_->size(db_size);
    face_id = 0;

    if (db_size != 0) {
      static std::vector<QueryResult> results;
      results.clear();

      SZ_RETCODE ret = unknown_database_->query(feature, 1, results);
      if (SZ_RETCODE_OK == ret && results[0].score >= 0.8) {
        face_id = results[0].face_id;
        unknown_database_->add(face_id, feature);
      }
    }

    if (duplicated_counter_ != 0) {
      duration = SECONDS_DIFF(current_query_clock, last_query_clock_);
      if (duration >
          std::max(cfg.duplication_interval, AudioTask::duration(person))) {
        last_query_clock_ = current_query_clock;
      } else
        ret = true;

    } else {
      if (face_id == 0) face_id = (db_size % 100) + 1;

      unknown_database_->add(face_id, feature);
      last_query_clock_ = current_query_clock;
    }
  }
  return ret;
}

bool RecordTask::if_temperature_updated(float &temperature) {
  if (((GOOD_TEMPERATURE(latest_temperature_) &&
        !GOOD_TEMPERATURE(temperature)) ||
       latest_temperature_ == 0) &&
      temperature > 0) {
    latest_temperature_ = temperature;
    return true;
  } else {
    temperature = latest_temperature_;
    return false;
  }
}

void RecordTask::rx_temperature(float body_temperature) {
  temperature_history_.push_back(body_temperature);
  // if (has_unhandle_person_) {
  //   is_running_ = true;
  //   update_person_temperature(duplicated_id_, duplicated_duration_,
  //                             latest_person_);
  //   has_unhandle_person_ = false;
  //   duplicated_counter_++;
  //   emit tx_display(latest_person_, false, false);
  // }
  // is_running_ = false;
}

void RecordTask::rx_card_readed(QString card_no) {
  if (!has_card_no_) {
    has_card_no_ = true;
    card_no_ = card_no.toStdString();
    SZ_LOG_INFO("card no={}", card_no_);

    rx_reset();
  }
}

void RecordTask::rx_enable(bool enable) {
  is_enabled_ = enable;
  rx_reset();
}

void RecordTask::rx_reset() {
  reset_recognize();
  reset_temperature();

  duplicated_counter_ = 0;
  has_unhandle_person_ = false;
}
