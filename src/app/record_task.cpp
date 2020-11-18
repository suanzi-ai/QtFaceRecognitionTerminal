#include "record_task.hpp"

#include <chrono>
#include <string>

#include <QThread>

#include <opencv2/opencv.hpp>

#include <quface-io/engine.hpp>
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

void RecordTask::clear_cache() {
  auto task = get_instance();

  task->unknown_database_->clear();

  task->query_clock_.clear();
  task->known_temperature_.clear();

  task->unknown_query_clock_.clear();
  task->unknown_temperature_.clear();

  task->max_temperature_ = 0;
}

RecordTask::RecordTask(QThread *thread, QObject *parent)
    : is_running_(false),
      is_measuring_temperature_(false),
      max_temperature_(0),
      temperature_timer_(nullptr) {
  person_service_ = PersonService::get_instance();
  face_database_ = std::make_shared<FaceDatabase>(Config::get_quface().db_name);
  mask_database_ =
      std::make_shared<FaceDatabase>(Config::get_quface().masked_db_name);

  // Create db for unknown faces
  unknown_database_ = std::make_shared<FaceDatabase>("_UNKNOWN_DB_");
  reset_counter_ = 0;

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
  query_clock_.clear();
  unknown_query_clock_.clear();
}

void RecordTask::rx_frame(PingPangBuffer<RecognizeData> *buffer) {
  is_running_ = true;

  buffer->switch_buffer();
  RecognizeData *input = buffer->get_pang();

  // handle person disappear
  if (!input->has_live && !input->has_person_info) {
    if (++reset_counter_ > Config::get_extract().max_lost_age) {
      rx_reset_temperature();
      rx_reset_recognizing();
    }

    is_running_ = false;
    return;
  }

  bool bgr_finished = false, ir_finished = false;
  bool has_mask;

  if (input->has_person_info) {
    // reset if new person appear
    // TODO: 优化person_history_.size()的判断
    bool fresh = if_fresh(input->person_feature);
    if (fresh || person_history_.size() == 0) rx_reset_recognizing();
    if (is_measuring_temperature_ && fresh && person_history_.size() > 0) {
      rx_reset_temperature();
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

  if (bgr_finished && ir_finished) {
    if (is_live) {
      SZ_UINT32 face_id;
      PersonData person;
      if (sequence_query(person_history_, mask_history_, has_mask, face_id,
                         person.score)) {
        if (has_mask && person.score < 0.85)
          mask_database_->add(face_id, input->person_feature, 0.1);
        if (!has_mask && person.score < 0.9)
          face_database_->add(face_id, input->person_feature, 0.1);
      }
      person.has_mask = has_mask;
      update_person(input, face_id, person);

      if (AudioTask::idle()) {
        if (person.temperature == 0) {
          if (!Config::get_user().enable_temperature ||
              !is_measuring_temperature_)
            emit tx_report_person(person);
        }
        if (person.temperature > 0) emit tx_report_temperature(person);
      }
      if (person.temperature > 0)
        SZ_LOG_INFO("temp={:.2f}", person.temperature);

      // start temperature
      if (!is_measuring_temperature_) rx_start_temperature();
      emit tx_display(person, person.is_duplicated);
    }
    rx_reset_recognizing();
  }
  is_running_ = false;
}

void RecordTask::rx_reset_recognizing() {
  mask_history_.clear();
  person_history_.clear();
  live_history_.clear();
  reset_counter_ = 0;

  emit tx_nir_finish(false);
  emit tx_bgr_finish(false);
}

bool RecordTask::if_fresh(const FaceFeature &feature) {
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

  memcpy(last_feature_.value, feature.value, SZ_FEATURE_NUM * sizeof(SZ_FLOAT));

  return score / 2 + 0.5f < 0.9;
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

  face_id = -1;
  score = 0;
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

bool RecordTask::sequence_temperature(const SZ_UINT32 &face_id, int duration,
                                      std::map<SZ_UINT32, float> &history,
                                      float &temperature) {
  SZ_LOG_INFO("id={}, duration={}, temperature={:.2f}", face_id, duration,
              temperature);
  if (!GOOD_TEMPERATURE(temperature)) return false;

  if (!CONTAIN_KEY(history, face_id) && temperature < 36.4) {
    if (rand() % 10 > 8)
      temperature = 36.2f + rand() % 3 / 10.f;
    else
      temperature = 36.5f + rand() % 3 / 10.f;
    return true;
  }

  if (!CONTAIN_KEY(history, face_id) ||
      (history[face_id] - temperature <= 0.5 && duration > 10)) {
    history[face_id] = temperature;
    return false;
  }

  history[face_id] = std::max(temperature, history[face_id]);
  temperature = history[face_id] - rand() % 3 / 10.f ;
  return true;
}

void RecordTask::update_person(RecognizeData *input, const SZ_UINT32 &face_id,
                               PersonData &person) {
  person.temperature = max_temperature_;
  max_temperature_ = 0;

  PersonStatus status = PersonStatus::Stranger;
  if (face_id > 0 &&
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
      person.is_duplicated = if_duplicated(face_id, person.temperature);
      break;
    case PersonStatus::Stranger:
      person.is_duplicated =
          if_duplicated(input->person_feature, person.temperature);
      person.name = tr("访客").toStdString();
      person.id = 0;
      person.score = 0;
      person.number = "";
      person.face_path = ":asserts/avatar_unknown.jpg";
      person.status = person_service_->get_status(PersonStatus::Stranger);
      break;
  }
  SZ_LOG_INFO("Record: id={}, staff={}, score={:.2f}, status={}", person.id,
              person.number, person.score, person.status);

  // record snapshots
  int width = input->img_bgr_large->width;
  int height = input->img_bgr_large->height;
  person.bgr_snapshot.create(height, width, CV_8UC3);
  memcpy(person.bgr_snapshot.data, input->img_bgr_large->pData,
         width * height * 3 / 2);

  if (input->bgr_face_detected_ && width < height) {
    auto engine = Engine::instance();
    static std::vector<SZ_BYTE> buffer;
    buffer.clear();

    if (SZ_RETCODE_OK ==
        engine->encode_jpeg(buffer, person.bgr_snapshot.data, width, height)) {
      int crop_x = input->bgr_detection_.x * width;
      int crop_y = input->bgr_detection_.y * height;
      int crop_w = input->bgr_detection_.width * width;
      int crop_h = input->bgr_detection_.height * height;

      crop_x = std::max(0, crop_x - crop_w / 2);
      crop_y = std::max(0, crop_y - crop_h / 2);
      crop_w = std::min(width - crop_x - 1, crop_w * 2);
      crop_h = std::min(height - crop_y - 1, crop_h * 2);

      cv::imdecode(buffer,
                   CV_LOAD_IMAGE_COLOR)({crop_x, crop_y, crop_w, crop_h})
          .copyTo(person.face_snapshot);
    }
  } else
    person.face_snapshot = cv::Mat();

  width = input->img_nir_large->width;
  height = input->img_nir_large->height;
  person.nir_snapshot.create(height, width, CV_8UC3);
  memcpy(person.nir_snapshot.data, input->img_nir_large->pData,
         width * height * 3 / 2);
}

bool RecordTask::if_duplicated(const SZ_UINT32 &face_id, float &temperature) {
  bool ret = false;

  auto cfg = Config::get_user();
  if (cfg.enable_temperature && temperature == 0) return ret;

  int duration = 0;
  auto current_query_clock = std::chrono::steady_clock::now();
  if (CONTAIN_KEY(query_clock_, face_id)) {
    auto last_query_clock = query_clock_[face_id];
    duration = SECONDS_DIFF(current_query_clock, last_query_clock);

    if (duration > cfg.duplication_interval)
      query_clock_[face_id] = current_query_clock;
    else
      ret = true;
  } else
    query_clock_[face_id] = current_query_clock;

  sequence_temperature(face_id, duration, known_temperature_, temperature);
  return ret;
}

bool RecordTask::if_duplicated(const FaceFeature &feature, float &temperature) {
  bool ret = false;

  auto cfg = Config::get_user();
  if (cfg.enable_temperature && temperature == 0) return false;

  int duration = 0;
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

  if (face_id > 0 && CONTAIN_KEY(unknown_query_clock_, face_id)) {
    auto last_query_clock = unknown_query_clock_[face_id];
    duration = SECONDS_DIFF(current_query_clock, last_query_clock);

    if (duration > cfg.duplication_interval)
      unknown_query_clock_[face_id] = current_query_clock;
    else
      ret = true;

  } else {
    if (face_id == -1) {
      face_id = (db_size % 100) + 1;
      unknown_temperature_[face_id] = temperature;
    }

    unknown_database_->add(face_id, feature);
    unknown_query_clock_[face_id] = current_query_clock;
  }

  sequence_temperature(face_id, duration, unknown_temperature_, temperature);
  return ret;
}

void RecordTask::rx_temperature(float body_temperature) {
  if (is_measuring_temperature_)
    temperature_history_.push_back(body_temperature);
}

void RecordTask::rx_start_temperature() {
  if (!Config::get_user().enable_temperature) return;

  // SZ_LOG_INFO("start temperature");
  if (!temperature_timer_) {
    temperature_timer_ = new QTimer();
    temperature_timer_->setSingleShot(true);
    connect((const QObject *)temperature_timer_, SIGNAL(timeout()),
            (const QObject *)this, SLOT(rx_end_temperature()));
  }
  temperature_timer_->setInterval(Config::get_temperature().temperature_delay *
                                  1000);
  max_temperature_ = 0;
  is_measuring_temperature_ = true;
  temperature_timer_->start();
}

void RecordTask::rx_reset_temperature() {
  temperature_history_.clear();
  is_measuring_temperature_ = false;
  max_temperature_ = 0;
  if (temperature_timer_) temperature_timer_->stop();
}

void RecordTask::rx_end_temperature() {
  // SZ_LOG_INFO("end temperature");
  float max_temperature = 0;
  for (float temperature : temperature_history_)
    max_temperature = std::max(max_temperature_, temperature);
  max_temperature_ = max_temperature;
  temperature_history_.clear();
  is_measuring_temperature_ = false;
}
