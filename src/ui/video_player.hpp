#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QScopedPointer>
#include <QWidget>

#include "antispoof_task.hpp"
#include "camera_reader.hpp"
#include "config.hpp"
#include "detect_task.hpp"
#include "detect_tip_widget.hpp"
#include "detection_float.h"
#include "image_package.h"
#include "recognize_data.hpp"
#include "pingpang_buffer.h"
#include "recognize_task.hpp"
#include "recognize_tip_widget.hpp"

namespace suanzi {

class VideoPlayer : public QWidget {
  Q_OBJECT

 public:
  VideoPlayer(FaceDatabasePtr db, FaceDetectorPtr detector,
              FaceExtractorPtr extractor, Config::ptr config,
              QWidget *parent = nullptr);
  ~VideoPlayer();

 protected:
  void paintEvent(QPaintEvent *event) override;

 private slots:
  void init_widgets();

 private:
  // TODO: gloabal configuration
  const QRect RECOGNIZE_TIP_BOX = {0, 1000, 800, 280};

  DetectTipWidget *detect_tip_widget_;
  RecognizeTipWidget *recognize_tip_widget_;

  CameraReader *camera_reader_0_;
  CameraReader *camera_reader_1_;

  AntispoofTask *antispoof_task_;
  DetectTask *detect_task_;
  RecognizeTask *recognize_task_;

  suanzi::Config::ptr config_;
  FaceDatabasePtr db_;
  FaceDetectorPtr detector_;
  FaceExtractorPtr extractor_;
};

}  // namespace suanzi

#endif
