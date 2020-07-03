#include "detect_tip_widget.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#define MOVING_AVERAGE(a, b, r) ((r) * (a) + (1 - (r)) * (b))

using namespace suanzi;

DetectTipWidget::DetectTipWidget(QWidget *parent) : QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  palette.setColor(QPalette::Foreground, Qt::green);
  setPalette(palette);

  is_updated_ = false;
}

DetectTipWidget::~DetectTipWidget() {}

void DetectTipWidget::paint(QPainter *painter) {
  if (is_updated_) {
    painter->drawRect(rect_);
    for (int i = 0; i < 5; i++) {
      painter->drawPoint(landmark_[i][0], landmark_[i][1]);
    }
  }
}

void DetectTipWidget::rx_display(DetectionFloat detection) {

  // TODO: add global configuration
  const int w = 800-1;
  const int h = 1280-1;

  is_updated_ = detection.b_valid;
  if (is_updated_) {
    QRect current_rect(detection.x * w, detection.y * h, detection.width * w,
                       detection.height * h);

    // Apply moving average to face bbox if intersects
    if (rect_.intersects(current_rect)) {
      rect_.setX(
          MOVING_AVERAGE(detection.x * w, rect_.x(), MOVING_AVERAGE_RATIO));
      rect_.setY(
          MOVING_AVERAGE(detection.y * h, rect_.y(), MOVING_AVERAGE_RATIO));
      rect_.setWidth(MOVING_AVERAGE(detection.width * w, rect_.width(),
                                    MOVING_AVERAGE_RATIO));
      rect_.setHeight(MOVING_AVERAGE(detection.height * h, rect_.height(),
                                     MOVING_AVERAGE_RATIO));
    } else
      rect_ = current_rect;

    for (int i = 0; i < 5; i++) {
      landmark_[i][0] = detection.landmark[i][0] * w;
      landmark_[i][1] = detection.landmark[i][1] * h;
    }
  }

  QWidget *p = (QWidget *)parent();
  p->update();
}

void DetectTipWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.drawRect(rect_);
}
