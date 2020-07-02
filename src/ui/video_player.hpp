#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QScopedPointer>
#include <QWidget>

#include "alive_task.h"
#include "camera_reader.h"
#include "detect_task.h"
#include "detection_float.h"
#include "detect_tip_widget.hpp"
#include "image_package.h"
#include "pingpangbuffer.h"
#include "recognize_task.h"
#include "recognizetipwidget.h"
#include "videoframewidget.h"

namespace suanzi {

class VideoPlayer : public QWidget {
  Q_OBJECT

 public:
  VideoPlayer(QWidget *parent = nullptr);
  ~VideoPlayer();

 protected:
  void paintEvent(QPaintEvent *event) override;

 private slots:
  void init_widgets();

 private:
  const QRect RECOGNIZE_BOX = {150, 300, 500, 500};
  const QRect RECOGNIZE_TIP_BOX = {150, 200, 500, 100};

  VideoFrameWidget *video_frame_widget_;
  DetectTipWidget *detect_tip_widget_;
  RecognizeTipWidget *recognize_tip_widget_;

  CameraReader *camera_reader_0_;
  CameraReader *camera_reader_1_;

  AliveTask *antispoof_task_;
  DetectTask *detect_task_;
  RecognzieTask *recognize_task_;
};

}  // namespace suanzi

#endif
