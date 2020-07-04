#include "detect_tip_widget.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

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
  if (rects_.size() > 0) {
    float sum_x = 0, sum_y = 0, sum_width = 0, sum_height = 0;
    float count = 0;

    auto it = rects_.end();
    QRect latest = rects_.back();
    do {
      it -= 1;

      QRect inter_section = latest.intersected(*it);
      QRect united_section = latest.united(*it);

      float iou = 1.f * inter_section.width() * inter_section.height() /
                  united_section.width() / united_section.height();

      float weight = pow(iou, 6);

      sum_x += weight * it->x();
      sum_y += weight * it->y();
      sum_width += weight * it->width();
      sum_height += weight * it->height();
      count += weight;

    } while (it != rects_.begin());

    painter->drawRect(QRect(sum_x / count, sum_y / count, sum_width / count,
                            sum_height / count));
  }
}

void DetectTipWidget::rx_display(DetectionFloat detection) {
  // TODO: add global configuration
  const int w = 800 - 1;
  const int h = 1280 - 1;

  is_updated_ = detection.b_valid;
  if (is_updated_) {
    // resize to square
    float center_x = (detection.x + .5f * detection.width) * w;
    float center_Y = (detection.y + .5f * detection.height) * h;
    float size = .5f * (detection.width * w + detection.height * h);

    QRect next_rect(center_x - .5f * size, center_Y - .5f * size, size, size);
    rects_.push_back(next_rect);
    if (rects_.size() > MAX_RECT_COUNT) rects_.erase(rects_.begin());

  } else {
    static int lost_age = 0;
    if (++lost_age > MAX_LOST_AGE) {
      rects_.clear();
      lost_age = 0;
    }
  }

  QWidget *p = (QWidget *)parent();
  p->update();
}

void DetectTipWidget::paintEvent(QPaintEvent *event) {}
