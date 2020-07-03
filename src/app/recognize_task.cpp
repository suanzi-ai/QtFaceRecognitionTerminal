#include "recognize_task.hpp"

#include <QThread>

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include "logger.hpp"

using namespace suanzi;

RecognzieTask::RecognzieTask(QThread *thread, QObject *parent) {
  // TODO: global configuration
  face_extractor_ = new suanzi::FaceExtractor("facemodel.bin");
  face_database_ = new suanzi::FaceDatabase("quface");

  lost_age_ = 0;

  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

RecognzieTask::~RecognzieTask() {
  if (face_extractor_) delete face_extractor_;
}

void RecognzieTask::rx_frame(PingPangBuffer<ImagePackage> *buffer,
                             DetectionFloat detection) {
  ImagePackage *pang = buffer->get_pang();

  if (detection.b_valid) {
    lost_age_ = 0;
  
    // crop in large image
    int width = pang->img_bgr_large->width;
    int height = pang->img_bgr_large->height;
    suanzi::FaceDetection face_detection =
        to_detection(detection, width, height);

    // 25ms
    suanzi::FaceFeature feature;
    face_extractor_->extract(
        (const SVP_IMAGE_S *)pang->img_bgr_large->pImplData, face_detection,
        feature);

    buffer->switch_buffer();
    emit tx_finish();

    // push query result to history
    std::vector<suanzi::QueryResult> results;
    if (SZ_RETCODE_OK == face_database_->query(feature, 1, results))
      history_.push_back(results[0]);

    // deduce recognize results
    if (history_.size() >= HISTORY_SIZE) {
      int face_id;

      if (query(history_, face_id)) {
        SZ_LOG_INFO("recognized: face_id = {}", face_id);
      } else {
        SZ_LOG_INFO("recognized: unknown", face_id);
      }

      history_.clear();
    }

  } else {
    // no face
    lost_age_ += 1;
    if (lost_age_ >= MAX_LOST_AGE) {
      history_.clear();
      lost_age_ = 0;
    }
  }
}

suanzi::FaceDetection RecognzieTask::to_detection(
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

bool RecognzieTask::query(std::vector<suanzi::QueryResult> history,
                          int &face_id) {
  std::map<int, int> person_counts;
  std::map<int, float> person_accumulate_score;
  std::map<int, float> person_max_score;

  // initialize map
  for (auto &person : history_) {
    person_counts[person.face_id] = 0;
    person_accumulate_score[person.face_id] = 0.f;
    person_max_score[person.face_id] = 0.f;
  }

  // accumulate id and score
  for (auto &person : history_) {
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

  int max_person_id = max_person->first;
  int max_count = max_person->second;
  float max_person_accumulate_score = person_accumulate_score[max_person_id];
  float max_person_score = person_max_score[max_person_id];

  if (max_count >= MIN_RECOGNIZE_COUNT &&
      max_person_accumulate_score >= MIN_ACCUMULATE_SCORE &&
      max_person_score >= MIN_RECOGNIZE_SCORE) {

    SZ_LOG_DEBUG("id={}, count={}, accumulate_score={}",
                 max_person_id, max_count,
                 person_accumulate_score[max_person_id]);

    face_id = max_person_id;
    return true;
  }
  else {
    return false;
  }
}
