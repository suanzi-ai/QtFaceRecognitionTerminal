#include "recognize_task.hpp"

#include <QThread>

#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

using namespace suanzi;

RecognzieTask::RecognzieTask(QThread *pThread, QObject *parent) {
  face_extractor_ = new suanzi::FaceExtractor("facemodel.bin");

  if (pThread == nullptr) {
    static QThread thread;
    moveToThread(&thread);
    thread.start();
  } else {
    moveToThread(pThread);
    pThread->start();
  }
}

RecognzieTask::~RecognzieTask() {
  if (face_extractor_) delete face_extractor_;
}

void RecognzieTask::rxFrame(PingPangBuffer<ImagePackage> *buffer,
                            DetectionFloat detection) {
  ImagePackage *pPang = buffer->get_pang();
  // printf("RecognzieTask threadId=%x   %x %d\n", QThread::currentThreadId(),
  // pPang, pPang->frame_idx);
  QThread::msleep(10);

  suanzi::FaceDetection face_detection;
  int w = pPang->img_bgr_large->width;
  int h = pPang->img_bgr_large->height;
  if (detection.b_valid) {
    face_detection.bbox.x = detection.x * w;
    face_detection.bbox.y = detection.y * h;
    face_detection.bbox.width = detection.width * w;
    face_detection.bbox.height = detection.height * h;
    for (int i = 0; i < 5; i++) {
      face_detection.landmarks.point[i].x = detection.landmark[i][0] * w;
      face_detection.landmarks.point[i].y = detection.landmark[i][1] * h;
    }

    printf("frame:%d  %f %f %f %f \n", pPang->frame_idx, face_detection.bbox.x,
           face_detection.bbox.y, face_detection.bbox.width,
           face_detection.bbox.height);

    std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

    suanzi::FaceFeature feature;
    face_extractor_->extract(
        (const SVP_IMAGE_S *)pPang->img_bgr_large->pImplData, face_detection,
        feature);

    std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> time_span =
        std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1);
    std::cout << "extract: "
              << ": \t" << time_span.count() << "\tseconds." << std::endl;
  }

  //
  static bool bInit = false;
  if (!bInit) {
    static Person person;
    person.name = "hahaha";
    emit tx_result(person);
    bInit = true;
  }

  // end current frame
  buffer->switch_buffer();
}
