#include "anti_spoofing_task.hpp"

#include <QThread>

using namespace suanzi;

AntiSpoofingTask::AntiSpoofingTask(FaceAntiSpoofingPtr anti_spoofing,
                                   Config::ptr config, QThread *thread,
                                   QObject *parent)
    : anti_spoofing_(anti_spoofing), config_(config) {
  if (thread == nullptr) {
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
  } else {
    moveToThread(thread);
    thread->start();
  }
}

AntiSpoofingTask::~AntiSpoofingTask() {}

void AntiSpoofingTask::rx_finish() { b_tx_ok_ = true; }

void AntiSpoofingTask::rx_frame(PingPangBuffer<RecognizeData> *buffer) {
  // SZ_LOG_DEBUG("rx_frame");
  RecognizeData *pang = buffer->get_pang();
  if (pang->nir_detection.b_valid) {
    int width = pang->img_nir_large->width;
    int height = pang->img_nir_large->height;
    FaceDetection face_detection =
        pang->nir_detection.to_detection(width, height);
    // SZ_LOG_DEBUG("image w={},h={}", width, height);
    // SZ_LOG_DEBUG("face_detection x={},y={},w={},h={}", face_detection.bbox.x,
    //              face_detection.bbox.y, face_detection.bbox.width,
    //              face_detection.bbox.height);

    SZ_BOOL is_live = false;
    SZ_RETCODE ret = anti_spoofing_->validate(
        (const SVP_IMAGE_S *)pang->img_nir_large->pImplData, face_detection,
        is_live);
    if (ret == SZ_RETCODE_OK) {
      history_.push_back(is_live);
      while (history_.size() >= config_->liveness.history_size) {
        history_.erase(history_.begin());
      }
      pang->is_alive = is_person_alive();
      SZ_LOG_DEBUG("Liveness instant={}, history={}", is_live, pang->is_alive);

      if (b_tx_ok_) {
        b_tx_ok_ = false;
        b_data_ready_ = false;
        emit tx_frame(buffer);
      } else {
        b_data_ready_ = true;
      }
    } else {
      SZ_LOG_ERROR("Anti spoofing error %d", ret);

      if (b_tx_ok_ && b_data_ready_) {
        b_tx_ok_ = false;
        b_data_ready_ = false;
        emit tx_frame(buffer);
      } else {
        emit tx_no_frame();
      }
    }
  }
  buffer->switch_buffer();
  emit tx_finish();
}

void AntiSpoofingTask::rx_no_frame() { emit tx_no_frame(); }

bool AntiSpoofingTask::is_person_alive() {
  if (history_.size() < config_->liveness.min_alive_count) {
    return false;
  }

  int liveness_count = 0;
  int max_liveness_lost_count = 0;
  int liveness_lost_count = 0;
  for (bool alive : history_) {
    if (alive) {
      liveness_count++;

      if (liveness_lost_count > max_liveness_lost_count) {
        max_liveness_lost_count = liveness_lost_count;
      }
      liveness_lost_count = 0;
    } else {
      liveness_lost_count++;
    }
  }

  SZ_LOG_DEBUG("liveness count = {}, lost = {}", liveness_count,
               max_liveness_lost_count);

  return liveness_count >= config_->liveness.min_alive_count &&
         max_liveness_lost_count <= config_->liveness.continuous_max_lost_count;
}
