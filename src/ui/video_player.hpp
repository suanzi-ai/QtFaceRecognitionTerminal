#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>

#include "config.hpp"
#include "image_package.hpp"
#include "pingpang_buffer.hpp"
#include "recognize_data.hpp"

#include "audio_task.hpp"
#include "camera_reader.hpp"
#include "detect_task.hpp"
#include "face_timer.hpp"
#include "recognize_task.hpp"
#include "record_task.hpp"
#include "upload_task.hpp"

#include "detect_tip_widget.hpp"
#include "outline_widget.hpp"
#include "recognize_tip_widget.hpp"
#include "screen_saver_widget.hpp"
#include "status_banner.hpp"
#include "temperature_task.hpp"
// #include "isp_hist_widget.hpp"

namespace suanzi {

class VideoPlayer : public QWidget {
  Q_OBJECT

 public:
  VideoPlayer(FaceDatabasePtr db, FaceDetectorPtr detector,
              FacePoseEstimatorPtr pose_estimator, FaceExtractorPtr extractor,
              FaceAntiSpoofingPtr anti_spoofing,
              PersonService::ptr person_service, QWidget *parent = nullptr);
  ~VideoPlayer();

 protected:
  void init_workflow();
  void init_widgets();

  void paintEvent(QPaintEvent *event) override;

 private:
  FaceDatabasePtr db_;
  FaceDetectorPtr detector_;
  FacePoseEstimatorPtr pose_estimator_;
  FaceExtractorPtr extractor_;
  FaceAntiSpoofingPtr anti_spoofing_;

  DetectTipWidget *detect_tip_widget_bgr_;
  DetectTipWidget *detect_tip_widget_nir_;
  RecognizeTipWidget *recognize_tip_widget_;

  ScreenSaverWidget *screen_saver_;
  OutlineWidget *outline_widget_;
  StatusBanner *status_banner_;
  // HeatMapWidget *heat_map_widget_;
  // ISPHistWidget *isp_hist_widget_;

  CameraReader *camera_reader_;

  DetectTask *detect_task_;
  RecognizeTask *recognize_task_;
  RecordTask *record_task_;
  FaceTimer *face_timer_;

  UploadTask *upload_task_;
  AudioTask *audio_task_;

  TemperatureTask *temperature_task_;
  PersonService::ptr person_service_;
};

}  // namespace suanzi

#endif
