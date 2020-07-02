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

DetectTask::DetectTask(QThread *thread, QObject *parent) {
  // TODO: global configuration for model path
  face_detector_ = new suanzi::FaceDetector("facemodel.bin");

  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

DetectTask::~DetectTask() {
  if (face_detector_) delete face_detector_;
}

void DetectTask::rx_frame(PingPangBuffer<ImagePackage> *buffer) {
  ImagePackage *pPang = buffer->get_pang();
  printf("DetectTask0 threadId=%x  %x %d\n", QThread::currentThreadId(),
  pPang,
  pPang->frame_idx);
  //QThread::msleep(1000);

  // std::chrono::steady_clock::time_point t1 =
  // std::chrono::steady_clock::now();

  std::vector<suanzi::FaceDetection> detections;
  // 256x256  7ms
  face_detector_->detect((const SVP_IMAGE_S *)pPang->img_bgr_small->pImplData,
                         detections);

  // std::chrono::steady_clock::time_point t2 =
  // std::chrono::steady_clock::now(); std::chrono::duration<double> time_span =
  //     std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
  // std::cout << "det: "
  //           << ": \t" << time_span.count() << "\tseconds." << std::endl;

  for (int i = 0; i < detections.size(); i++) {
    auto rect = detections[i].bbox;
    printf("det %d: %0.2f %0.2f %0.2f %0.2f \n", pPang->frame_idx, rect.x,
           rect.y, rect.width, rect.height);
    break;
  }

  // find largest bbox
  int max_id = 0;
  float max_area = detections[0].bbox.width * detections[0].bbox.height;
  for (int i = 1; i < detections.size(); i++) {
    float area = detections[i].bbox.width * detections[i].bbox.height;
    if (area > max_area) {
      max_id = i;
      max_area = area;
    }
  }
    printf("DetectTask1 threadId=%x  %x %d\n", QThread::currentThreadId(),
  pPang,
  pPang->frame_idx);
  buffer->switch_buffer();
  emit tx_finish();

  // TODO
  // bgr and nir face detection
  // 2 detection results combine
  // send msgs for next steps

  DetectionFloat detection_bgr;
  detection_bgr.frame_idx = image->frame_idx;
  // DetectionFloat detection_nir;
  if (detections.size() > 0) {
    int w = image->img_bgr_small->width-1;
    int h = image->img_bgr_small->height-1;
    auto rect = detections[max_id].bbox;
    detection_bgr.x = rect.x * 1.0 / w;
    detection_bgr.y = rect.y * 1.0 / h;
    detection_bgr.width = rect.width * 1.0 / w;
    detection_bgr.height = rect.height * 1.0 / h;
    detection_bgr.b_valid = true;
    for (int i = 0; i < 5; i++) {
      detection_bgr.landmark[i][0] =
          detections[max_id].landmarks.point[i].x / w;
      detection_bgr.landmark[i][1] =
          detections[max_id].landmarks.point[i].y / h;
    }
  } else {
    detection_bgr.b_valid = false;
  }
  emit tx_detection_bgr(buffer, detection_bgr);
  // emit tx_detection_nir(buffer, detection_nir);
}
