#include "heatmap_widget.hpp"

#include <algorithm>
#include <cstring>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

HeatmapWidget::HeatmapWidget(int width, int height, QWidget *parent)
    : QWidget(parent), raw_(16, 16, CV_8UC3) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);

  x_ = y_ = 0.5;
  detection_.x = 0.45;
  detection_.y = 0.45;
  detection_.width = 0.1;
  detection_.height = 0.1;
}

HeatmapWidget::~HeatmapWidget() {}

void HeatmapWidget::rx_update(TemperatureMatrix mat, DetectionRatio detection,
                              float x, float y) {
  assert(mat.size == 256);

  min_ = max_ = mat.value[0];
  for (size_t i = 1; i < 256; i++) {
    min_ = std::min(min_, mat.value[i]);
    max_ = std::max(max_, mat.value[i]);
  }

  for (size_t i = 0; i < 256; i++) {
    int x = i % 16, y = i / 16;

    float value = mat.value[i];
    float ratio = 2 * (value - min_) / (max_ - min_);
    int r = std::max(0.f, 255 * (ratio - 1));
    int b = std::max(0.f, 255 * (1 - ratio));
    int g = 255 - b - r;

    cv::Vec3b &color = raw_.at<cv::Vec3b>(y, x);
    color[0] = r;
    color[1] = g;
    color[2] = b;
    raw_.at<cv::Vec3b>(y, x) = color;
  }

  heatmap_ = QPixmap::fromImage(
      QImage((unsigned char *)raw_.data, 16, 16, QImage::Format_RGB888));
  detection_ = detection;
  x_ = x;
  y_ = y;
}

void HeatmapWidget::paint(QPainter *painter) {
  const int w = width();
  const int h = height();

  const QRect target(0.0625 * w, 0.703125 * h, 0.1875 * w, 0.1171875 * h);

  if (Config::get_user().enable_temperature) {
    painter->drawPixmap(target, heatmap_, QRect());

    QRect face(target.x() + target.width() * detection_.x,
               target.y() + target.height() * detection_.y,
               target.width() * detection_.width,
               target.height() * detection_.height);
    painter->setPen(Qt::white);
    painter->drawRect(face);

    painter->drawLine(target.x() + (x_ - 0.1) * target.width(),
                      target.y() + y_ * target.height(),
                      target.x() + (x_ + 0.1) * target.width(),
                      target.y() + y_ * target.height());
    painter->drawLine(target.x() + x_ * target.width(),
                      target.y() + (y_ - 0.1) * target.height(),
                      target.x() + x_ * target.width(),
                      target.y() + (y_ + 0.1) * target.height());
  }
}
