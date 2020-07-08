#include "detect_task.hpp"

#include <QRect>
#include <QThread>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include "quface/common.hpp"
#include "quface/face.hpp"

using namespace suanzi;

DetectTask::DetectTask(FaceDetectorPtr detector, QThread *thread,
                       QObject *parent)
    : face_detector_(detector) {
  b_tx_ok_ = true;

  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

DetectTask::~DetectTask() {}

void DetectTask::rx_frame(PingPangBuffer<ImagePackage> *buffer) {
  ImagePackage *pang = buffer->get_pang();

  // 256x256  7ms
  std::vector<suanzi::FaceDetection> detections;
  face_detector_->detect((const SVP_IMAGE_S *)pang->img_bgr_small->pImplData,
                         detections);
  buffer->switch_buffer();
  emit tx_finish();

  // if face detected
  if (detections.size() > 0) {
    int width = pang->img_bgr_small->width;
    int height = pang->img_bgr_small->height;
    DetectionFloat largest_face = select_face(detections, width, height);
    emit tx_display(largest_face);

    // send detection if recognized finished
    if (b_tx_ok_) {
      b_tx_ok_ = false;
      emit tx_recognize(buffer, largest_face);
    }

  } else {
    DetectionFloat no_face;
    no_face.b_valid = false;
    emit tx_display(no_face);
    emit tx_recognize(buffer, no_face);
  }
}

void DetectTask::rx_finish() { b_tx_ok_ = true; }

DetectionFloat DetectTask::select_face(
    std::vector<suanzi::FaceDetection> &detections, int width, int height) {
  int max_id = 0;
  float max_area = detections[0].bbox.width * detections[0].bbox.height;
  for (int i = 1; i < detections.size(); i++) {
    float area = detections[i].bbox.width * detections[i].bbox.height;
    if (area > max_area) {
      max_id = i;
      max_area = area;
    }
  }

  DetectionFloat detection_bgr;
  auto rect = detections[max_id].bbox;
  detection_bgr.x = rect.x * 1.0 / width;
  detection_bgr.y = rect.y * 1.0 / height;
  detection_bgr.width = rect.width * 1.0 / width;
  detection_bgr.height = rect.height * 0.8 / height;  // remove neck
  detection_bgr.b_valid = true;
  for (int i = 0; i < 5; i++) {
    detection_bgr.landmark[i][0] =
        detections[max_id].landmarks.point[i].x / width;
    detection_bgr.landmark[i][1] =
        detections[max_id].landmarks.point[i].y / height;
  }

  return detection_bgr;
}
