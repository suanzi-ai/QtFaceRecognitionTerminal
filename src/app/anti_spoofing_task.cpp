#include "anti_spoofing_task.hpp"

#include <QThread>

using namespace suanzi;

AntiSpoofingTask::AntiSpoofingTask(FaceAntiSpoofingPtr anti_spoofing,
                                   QThread *thread, QObject *parent)
    : anti_spoofing_(anti_spoofing) {
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
  auto cfg = Config::get_liveness();
  // SZ_LOG_DEBUG("rx_frame");
  if (!Config::enable_anti_spoofing()) {
    emit tx_frame(buffer);
    buffer->switch_buffer();
    emit tx_finish();
    return;
  }

  RecognizeData *pang = buffer->get_pang();
  if (pang->nir_detection.b_valid) {
    int width = pang->img_nir_large->width;
    int height = pang->img_nir_large->height;
    memset(pang->img_nir_large->pData + width * height, 0x80,
           width * height / 2);
    FaceDetection face_detection =
        pang->nir_detection.to_detection(width, height);
    // SZ_LOG_DEBUG("image w={},h={}, frame={}", width, height,
    // pang->frame_idx); SZ_LOG_DEBUG("face_detection x={},y={},w={},h={}",
    // face_detection.bbox.x,
    //              face_detection.bbox.y, face_detection.bbox.width,
    //              face_detection.bbox.height);

    //  save nir images for training
#if 0
    if (pang->frame_idx % 5 == 0) {
      MmzImage *dest_img =
          new MmzImage(width, height, SZ_IMAGETYPE_BGR_PACKAGE);

      if (Ive::getInstance()->yuv2RgbPacked(dest_img, pang->img_nir_large,
                                            true)) {
        SZ_LOG_DEBUG("image w={},h={}, frame={}", width, height,
                     pang->frame_idx);

        cv::Mat big_nir(height, width, CV_8UC3, dest_img->pData);
        cv::imwrite(
            "nir_images/" + std::to_string(pang->frame_idx) + ".jpg",
            big_nir({face_detection.bbox.x, face_detection.bbox.y,
                     face_detection.bbox.width, face_detection.bbox.height})
                .clone());

        // cv::imwrite("nir_images/" + std::to_string(pang->frame_idx) + ".jpg",
        //             big_nir);
      }
      delete dest_img;
    }
#endif

    SZ_BOOL is_live = false;
    SZ_RETCODE ret = anti_spoofing_->validate(
        (const SVP_IMAGE_S *)pang->img_nir_large->pImplData, face_detection,
        is_live);
    if (ret == SZ_RETCODE_OK) {
      history_.push_back(is_live);
      while (history_.size() >= cfg.history_size) {
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
  auto cfg = Config::get_liveness();

  if (history_.size() < cfg.min_alive_count) {
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

  return liveness_count >= cfg.min_alive_count &&
         max_liveness_lost_count <= cfg.continuous_max_lost_count;
}
