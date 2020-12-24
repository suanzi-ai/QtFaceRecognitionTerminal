#ifndef HEATMAP_H
#define HEATMAP_H

#include <QFont>
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

 private:
  void paintEvent(QPaintEvent *event) override;

 public slots:
  void rx_init(int success);
  void rx_update(TemperatureMatrix mat, QRectF detection, float x, float y);

 private:
  QColor to_rgb(float value);

 private:
  QFont font_;

  float min_, max_;
  float x_, y_;

  cv::Mat raw_;
  bool init_;
  QPixmap heatmap_;
  QRectF detection_;
  int success_;
};

}  // namespace suanzi

#endif
