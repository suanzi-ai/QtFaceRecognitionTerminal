#include "record_task.hpp"

#include <QThread>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include <opencv2/opencv.hpp>

#include "logger.hpp"

using namespace suanzi;

RecordTask::RecordTask(
    PersonService::ptr person_service,
    MemoryPool<ImageBuffer, sizeof(ImageBuffer) * 5> *mem_pool, QObject *parent)
    : person_service_(person_service), mem_pool_(mem_pool) {
  static QThread new_thread;
  moveToThread(&new_thread);
  new_thread.start();
}

RecordTask::~RecordTask() {}

void RecordTask::rx_record(int face_id, ImageBuffer *buffer) {

  // TODO: Global image configuration
  int width = 224, height = 320;
  cv::Mat mat(height, width, CV_8UC3, buffer->data);
  std::vector<SZ_UINT8> image_buffer;

  if (cv::imencode(".jpg", mat, image_buffer))
    person_service_->report_face_record(face_id, image_buffer);
  else
    SZ_LOG_ERROR("Encode jpg failed");

  mem_pool_->deallocate(buffer);
}
