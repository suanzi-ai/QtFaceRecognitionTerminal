#include "recognize_task.hpp"

#include <QThread>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include "logger.hpp"

using namespace suanzi;

RecognizeTask::RecognizeTask(FaceDatabasePtr db, FaceExtractorPtr extractor,
                             PersonService::ptr person_service,
                             Config::ptr config, QThread *thread,
                             QObject *parent)
    : face_database_(db),
      face_extractor_(extractor),
      person_service_(person_service),
      config_(config) {
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

RecognizeTask::~RecognizeTask() {}

void RecognizeTask::rx_frame(PingPangBuffer<RecognizeData> *buffer) {
  RecognizeData *pang = buffer->get_pang();
  if (pang->detection.b_valid) {
    // crop in large image
    int width = pang->img_bgr_large->width;
    int height = pang->img_bgr_large->height;
    suanzi::FaceDetection face_detection =
        to_detection(pang->detection, width, height);

    // extract: 25ms
    suanzi::FaceFeature feature;
    face_extractor_->extract(
        (const SVP_IMAGE_S *)pang->img_bgr_large->pImplData, face_detection,
        feature);

    // push query result to history
    std::vector<suanzi::QueryResult> results;
    SZ_RETCODE ret = face_database_->query(feature, 1, results);
    if (SZ_RETCODE_OK == ret) {
      query_success(results[0], pang);
    } else if (SZ_RETCODE_EMPTY_DATABASE == ret) {
      query_empty_database();
    }
  }
  buffer->switch_buffer();
  emit tx_finish();
}

void RecognizeTask::rx_no_frame() {
	query_no_face();
}
							 
suanzi::FaceDetection RecognizeTask::to_detection(
    DetectionFloat detection_ratio, int width, int height) {
  suanzi::FaceDetection face_detection;
  face_detection.bbox.x = detection_ratio.x * width;
  face_detection.bbox.y = detection_ratio.y * height;
  face_detection.bbox.width = detection_ratio.width * width;
  face_detection.bbox.height = detection_ratio.height * height;

  for (int i = 0; i < 5; i++) {
    face_detection.landmarks.point[i].x =
        detection_ratio.landmark[i][0] * width;
    face_detection.landmarks.point[i].y =
        detection_ratio.landmark[i][1] * height;
  }

  return face_detection;
}

/*void RecognizeTask::report(SZ_UINT32 face_id, ImagePackage *img) {
  std::vector<SZ_UINT8> image_buffer;
  if (img->get_jpeg_buffer(img->img_bgr_small, image_buffer)) {
    SZ_RETCODE ret = person_service_->report_face_record(face_id, image_buffer);
    if (ret != SZ_RETCODE_OK) {
      SZ_LOG_ERROR("Report face record error");
    }
  } else {
    SZ_LOG_ERROR("Decode image failed");
  }
}*/

void RecognizeTask::query_success(const suanzi::QueryResult &person_info,
                                  RecognizeData *img) {
  history_.push_back(person_info);
  if (history_.size() >= config_->extract.history_size) {
    SZ_UINT32 face_id = 0;

    if (sequence_query(history_, face_id)) {
      suanzi::PersonData person;
      SZ_RETCODE ret = person_service_->get_person(face_id, person);
      if (ret == SZ_RETCODE_OK) {
        SZ_LOG_INFO("recognized: id = {}, name = {}", person.id, person.name);
        tx_display({person.number, person.name, person.face_path});
      }
    } else {
      SZ_LOG_INFO("recognized: unknown");
      tx_display({"", "访客", ":asserts/avatar_unknown.jpg"});
    }

    if (last_face_id_ != face_id) {
      //report(face_id, img);
      last_face_id_ = face_id;
    }

    history_.clear();
  }
}

void RecognizeTask::query_empty_database() {
  static int empty_age = 0;
  if (++empty_age >= config_->extract.history_size) {
    SZ_LOG_INFO("recognized: unknown (empty database)");
    tx_display({"", "访客", ":asserts/avatar_unknown.jpg"});

    history_.clear();
    empty_age = 0;
  }
}

void RecognizeTask::query_no_face() {
  static int lost_age = 0;
  if (++lost_age >= config_->extract.max_lost_age) {
    history_.clear();
    tx_display({});
    lost_age = 0;
  }
}

bool RecognizeTask::sequence_query(std::vector<suanzi::QueryResult> history,
                                   SZ_UINT32 &face_id) {
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
  if (max_count >= config_->extract.min_recognize_count &&
      max_person_accumulate_score >= config_->extract.min_accumulate_score &&
      max_person_score >= config_->extract.min_recognize_score) {
    face_id = max_person_id;
    return true;
  } else {
    return false;
  }
}
