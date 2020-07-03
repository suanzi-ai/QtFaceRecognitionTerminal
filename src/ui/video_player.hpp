#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QScopedPointer>
#include <QWidget>

#include "antispoof_task.hpp"
#include "camera_reader.h"
#include "detect_task.hpp"
#include "detect_tip_widget.hpp"
#include "detection_float.h"
#include "image_package.h"
#include "pingpang_buffer.h"
#include "recognize_task.hpp"
#include "recognize_tip_widget.hpp"
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
  const QRect RECOGNIZE_TIP_BOX = {150, 300, 500, 500};

  VideoFrameWidget *video_frame_widget_;
  DetectTipWidget *detect_tip_widget_;
  RecognizeTipWidget *recognize_tip_widget_;

  CameraReader *camera_reader_0_;
  CameraReader *camera_reader_1_;

  AntispoofTask *antispoof_task_;
  DetectTask *detect_task_;
  RecognzieTask *recognize_task_;
};

}  // namespace suanzi

#endif
