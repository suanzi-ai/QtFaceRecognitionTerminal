#include "heatmap_widget.hpp"

#include <algorithm>
#include <cstring>

#include <QPaintEvent>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

HeatmapWidget::HeatmapWidget(int width, int height, QWidget *parent)
    : QWidget(parent), init_(false), success_(0) {
  // setAttribute(Qt::WA_StyledBackground, true);
  setStyleSheet("QWidget {background-color:rgb(0, 255, 0);}");
  setAutoFillBackground(true);

  int w = 0.1875 * width;
  int h = 0.1171875 * height;
  int pos_x = 0.0625 * width;
  int pos_y = 0.703125 * height;
  setFixedSize(w, h);
  move(pos_x, pos_y);

  font_.setPointSize(0.1 * w);

  x_ = y_ = 0.5;
  detection_.setRect(0.45, 0.45, 0.1, 0.1);

  // raw_ = cv::Scalar(0, 255, 0);
  // heatmap_ = QPixmap::fromImage(
  // QImage((unsigned char *)raw_.data, 16, 16, QImage::Format_RGB888));
  hide();
}

HeatmapWidget::~HeatmapWidget() {}

void HeatmapWidget::init(int temperature_width, int temperature_height) {
  if (raw_.empty()) {
    raw_ = cv::Mat(temperature_height, temperature_width, CV_8UC3);
    raw_ = cv::Scalar(0, 255, 0);
    heatmap_ =
        QPixmap::fromImage(QImage((unsigned char *)raw_.data, temperature_width,
                                  temperature_height, QImage::Format_RGB888));
  }
}

void HeatmapWidget::rx_init(int success) {
  success_ = success;
  if (Config::display_temperature()) show();
  update();
}

void HeatmapWidget::rx_update(TemperatureMatrix mat, QRectF detection, float x,
                              float y) {
  if (Config::display_temperature())
    show();
  else {
    hide();
    return;
  }

  assert(mat.size == 256 || mat.size == 1024);
  init(mat.width, mat.height);
  init_ = true;

  min_ = max_ = mat.value[0];
  for (size_t i = 1; i < mat.size; i++) {
    min_ = std::min(min_, mat.value[i]);
    max_ = std::max(max_, mat.value[i]);
  }

  for (size_t i = 0; i < mat.size; i++) {
    int x = i % mat.width, y = i / mat.width;

    float value = mat.value[i];
    float ratio = 2 * (value - min_) / (max_ - min_);
    int r = std::min(1.f, ratio) * 255;
    int g = std::min(1.f, 2 - ratio) * 255;
    int b = 0;

    cv::Vec3b &color = raw_.at<cv::Vec3b>(y, x);
    color[0] = r;
    color[1] = g;
    color[2] = b;
    raw_.at<cv::Vec3b>(y, x) = color;
  }

  heatmap_ = QPixmap::fromImage(QImage((unsigned char *)raw_.data, mat.width,
                                       mat.width, QImage::Format_RGB888));
  detection_ = detection;
  x_ = x;
  y_ = y;
  update();
}

void HeatmapWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  const int w = width();
  const int h = height();

  if (Config::display_temperature()) {
    const QRect target(0, 0, w, h);
    painter.drawPixmap(target, heatmap_, QRect());
    painter.setPen(Qt::white);
    if (init_) {
      QRect face(target.x() + target.width() * detection_.x(),
                 target.y() + target.height() * detection_.y(),
                 target.width() * detection_.width(),
                 target.height() * detection_.height());
      painter.drawRect(face);

      painter.drawLine(target.x() + (x_ - 0.1) * target.width(),
                       target.y() + y_ * target.height(),
                       target.x() + (x_ + 0.1) * target.width(),
                       target.y() + y_ * target.height());
      painter.drawLine(target.x() + x_ * target.width(),
                       target.y() + (y_ - 0.1) * target.height(),
                       target.x() + x_ * target.width(),
                       target.y() + (y_ + 0.1) * target.height());
    } else {
      char progress[100];
      sprintf(progress, "Loading %d%%", success_ * 10);
      painter.setFont(font_);
      painter.drawText(target.x() + 0.05 * target.width(),
                       target.y() + 0.55 * target.height(), progress);
    }
  }
}
