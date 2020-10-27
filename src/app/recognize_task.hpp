#ifndef RECOGNIZE_TASK_H
#define RECOGNIZE_TASK_H

#include <QObject>

#include "config.hpp"
#include "detection_data.hpp"
#include "pingpang_buffer.hpp"
#include "quface_common.hpp"
#include "recognize_data.hpp"

namespace suanzi {
class RecognizeTask : QObject {
  Q_OBJECT
 public:
  RecognizeTask(FaceDatabasePtr db, FaceExtractorPtr extractor,
                FaceAntiSpoofingPtr anti_spoofing,
                MaskDetectorPtr mask_detector, QThread *thread = nullptr,
                QObject *parent = nullptr);
  ~RecognizeTask();

 private slots:
  void rx_frame(PingPangBuffer<DetectionData> *buffer);
  void rx_nir_finish(bool if_finished);
  void rx_bgr_finish(bool if_finished);

 signals:
  void tx_finish();

  // for output
  void tx_frame(PingPangBuffer<RecognizeData> *buffer);

 private:
  bool is_live(DetectionData *detection);
  bool has_mask(DetectionData *detection);
  void extract_and_query(DetectionData *detection, FaceFeature &feature,
                         QueryResult &person_info);

  // nyy
  const Size VPSS_CH_SIZES_BGR[3] = {
      {1920, 1080}, {1080, 704}, {320, 224}};  // larger small
  const Size VPSS_CH_SIZES_NIR[3] = {
      {1920, 1080}, {1080, 704}, {320, 224}};  // larger small
  const int CH_INDEXES_BGR[3] = {0, 1, 2};
  const bool CH_ROTATES_BGR[3] = {false, true, true};
  const int CH_INDEXES_NIR[3] = {0, 1, 2};
  const bool CH_ROTATES_NIR[3] = {false, true, true};

  bool rx_nir_finished_;
  bool rx_bgr_finished_;

  FaceDatabasePtr face_database_;
  FaceExtractorPtr face_extractor_;
  FaceAntiSpoofingPtr anti_spoofing_;
  MaskDetectorPtr mask_detector_;

  RecognizeData *buffer_ping_, *buffer_pang_;
  PingPangBuffer<RecognizeData> *pingpang_buffer_;
};

}  // namespace suanzi

#endif
