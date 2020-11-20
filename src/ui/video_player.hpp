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
#include "gpio_task.hpp"
#include "led_task.hpp"
#include "recognize_task.hpp"
#include "record_task.hpp"
#include "temperature_task.hpp"
#include "upload_task.hpp"

#include "detect_tip_widget.hpp"
#include "outline_widget.hpp"
#include "recognize_tip_widget.hpp"
#include "screen_saver_widget.hpp"
#include "status_banner.hpp"
#include "heatmap_widget.hpp"
#include "isp_hist_widget.hpp"

namespace suanzi {

class VideoPlayer : public QWidget {
  Q_OBJECT

 public:
  VideoPlayer(QWidget *parent = nullptr);
  ~VideoPlayer();

 protected:
  void init_workflow();
  void init_widgets();

  void paintEvent(QPaintEvent *event) override;

 private slots:
 	void delay_init_widgets();

 private:
  DetectTipWidget *detect_tip_widget_bgr_;
  DetectTipWidget *detect_tip_widget_nir_;
  RecognizeTipWidget *recognize_tip_widget_;

  ScreenSaverWidget *screen_saver_;
  OutlineWidget *outline_widget_;
  StatusBanner *status_banner_;
  HeatmapWidget *heatmap_widget_;
  ISPHistWidget *isp_hist_widget_;

  CameraReader *camera_reader_;

  DetectTask *detect_task_;
  RecognizeTask *recognize_task_;
  RecordTask *record_task_;
  FaceTimer *face_timer_;

  UploadTask *upload_task_;
  AudioTask *audio_task_;
  GPIOTask *gpio_task_;
  LEDTask* led_task_;

  TemperatureTask *temperature_task_;
};

}  // namespace suanzi

#endif
