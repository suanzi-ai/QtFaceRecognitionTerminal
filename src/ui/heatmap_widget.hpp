#ifndef HEATMAP_H
#define HEATMAP_H

#include <QPainter>
#include <QPixmap>
#include <QWidget>

#include <opencv2/opencv.hpp>

#include <quface-io/temperature.hpp>

#include "detection_data.hpp"

namespace suanzi {

using namespace io;

class HeatmapWidget : public QWidget {
  Q_OBJECT

 public:
  HeatmapWidget(int width, int height, QWidget *parent = nullptr);
  ~HeatmapWidget() override;

  void paint(QPainter *painter);

 private slots:
  void rx_update(TemperatureMatrix mat, DetectionRatio detection, float x,
                 float y);

 private:
  QColor to_rgb(float value);

 private:
  float min_, max_;
  float x_, y_;

  cv::Mat raw_;
  bool init_;
  QPixmap heatmap_;
  DetectionRatio detection_;
};

}  // namespace suanzi

#endif
