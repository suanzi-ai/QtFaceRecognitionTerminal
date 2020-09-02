#include "recognize_task.hpp"

#include <QThread>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include <quface/logger.hpp>

using namespace suanzi;

RecognizeTask::RecognizeTask(FaceDatabasePtr db, FaceExtractorPtr extractor,
                             FaceAntiSpoofingPtr anti_spoofing, QThread *thread,
                             QObject *parent)
    : face_database_(db),
      face_extractor_(extractor),
      anti_spoofing_(anti_spoofing) {
  // Initialize PINGPANG buffer
  Size size_bgr_1 = VPSS_CH_SIZES_BGR[1];
  Size size_bgr_2 = VPSS_CH_SIZES_BGR[2];
  if (CH_ROTATES_BGR[1]) {
    size_bgr_1.height = VPSS_CH_SIZES_BGR[1].width;
    size_bgr_1.width = VPSS_CH_SIZES_BGR[1].height;
  }
  if (CH_ROTATES_BGR[2]) {
    size_bgr_2.height = VPSS_CH_SIZES_BGR[2].width;
    size_bgr_2.width = VPSS_CH_SIZES_BGR[2].height;
  }

  Size size_nir_1 = VPSS_CH_SIZES_NIR[1];
  Size size_nir_2 = VPSS_CH_SIZES_NIR[2];
  if (CH_ROTATES_NIR[1]) {
    size_nir_1.height = VPSS_CH_SIZES_NIR[1].width;
    size_nir_1.width = VPSS_CH_SIZES_NIR[1].height;
  }
  if (CH_ROTATES_NIR[2]) {
    size_nir_2.height = VPSS_CH_SIZES_NIR[2].width;
    size_nir_2.width = VPSS_CH_SIZES_NIR[2].height;
  }

  buffer_ping_ =
      new RecognizeData(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  buffer_pang_ =
      new RecognizeData(size_bgr_1, size_bgr_2, size_nir_1, size_nir_2);
  pingpang_buffer_ =
      new PingPangBuffer<RecognizeData>(buffer_ping_, buffer_pang_);

  // Create thread
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }

  rx_finished_ = true;
  rx_nir_finished_ = false;
  rx_bgr_finished_ = false;
}

RecognizeTask::~RecognizeTask() {
  if (buffer_ping_) delete buffer_ping_;
  if (buffer_pang_) delete buffer_pang_;
  if (pingpang_buffer_) delete pingpang_buffer_;
}

void RecognizeTask::rx_frame(PingPangBuffer<DetectionData> *buffer) {
  // copy from input to output
  buffer->switch_buffer();
  DetectionData *input = buffer->get_pang();
  RecognizeData *output = pingpang_buffer_->get_ping();
  input->copy_to(*output);

  output->bgr_face_detected_ = input->bgr_face_detected_;
  output->nir_face_detected_ = input->nir_face_detected_;
  output->bgr_detection_ = input->bgr_detection_;
  output->nir_detection_ = input->nir_detection_;
  output->has_live = !rx_nir_finished_;
  output->has_person_info = !rx_bgr_finished_;

  if (input->bgr_face_valid()) {
    if (output->has_live) {
      if (!Config::enable_anti_spoofing())
        output->is_live = true;
      else
        output->is_live = is_live(input);
    }
    if (output->has_person_info)
      extract_and_query(input, output->person_feature, output->person_info);
  } else {
    output->has_live = false;
    output->has_person_info = false;
  }

  if (rx_finished_) {
    rx_finished_ = false;
    emit tx_frame(pingpang_buffer_);
  } else {
    QThread::usleep(10);
  }
  emit tx_finish();
}

void RecognizeTask::rx_finish() { rx_finished_ = true; }

void RecognizeTask::rx_nir_finish(bool if_finished) {
  rx_nir_finished_ = if_finished;
}

void RecognizeTask::rx_bgr_finish(bool if_finished) {
  rx_bgr_finished_ = if_finished;
}

bool RecognizeTask::is_live(DetectionData *detection) {
  if (detection->nir_face_valid()) {
    int width = detection->img_nir_large->width;
    int height = detection->img_nir_large->height;

    suanzi::FaceDetection face_detection;
    suanzi::FacePose pose;
    detection->nir_detection_.scale(width, height, face_detection, pose);

    SZ_BOOL ret;
    if (SZ_RETCODE_OK !=
        anti_spoofing_->validate(
            (const SVP_IMAGE_S *)detection->img_nir_large->pImplData,
            face_detection, ret))
      return false;
    return ret == SZ_TRUE;
  } else
    return false;
}

void RecognizeTask::extract_and_query(DetectionData *detection,
                                      FaceFeature &feature,
                                      QueryResult &person_info) {
  int width = detection->img_bgr_large->width;
  int height = detection->img_bgr_large->height;

  suanzi::FaceDetection face_detection;
  suanzi::FacePose pose;
  detection->bgr_detection_.scale(width, height, face_detection, pose);

  // extract: 25ms
  SZ_RETCODE ret = face_extractor_->extract(
      (const SVP_IMAGE_S *)detection->img_bgr_large->pImplData, face_detection,
      pose, feature);
  if (SZ_RETCODE_OK == ret) {
    // query
    static std::vector<suanzi::QueryResult> results;
    results.clear();

    ret = face_database_->query(feature, 1, results);
    if (SZ_RETCODE_OK == ret) {
      person_info.score = results[0].score;
      person_info.face_id = results[0].face_id;

      return;
    }
  }

  // SZ_RETCODE_EMPTY_DATABASE or SZ_RETCODE_FAILED
  person_info.score = 0;
  person_info.face_id = 0;
}
