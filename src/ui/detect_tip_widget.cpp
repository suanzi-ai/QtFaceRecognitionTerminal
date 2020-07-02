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
  if (is_updated_) {
    painter->drawRect(rect_);
    for (int i = 0; i < 5; i++) {
      painter->drawPoint(landmark_[i][0], landmark_[i][1]);
    }
  }
}

void DetectTipWidget::rx_result(PingPangBuffer<ImagePackage> *img,
                                DetectionFloat detection) {
    int w = 800;
  int h = 1280;
  is_updated_ = detection.b_valid;
  if (is_updated_) {
    int x = detection.x * w;
    int y = detection.y * h;
    int width = detection.width * w;
    int height = detection.height * h;
    this->rect_ = QRect(x, y, width, height);
    printf("%d: %d %d %d %d \n", 0, x, y, width, height);

    for (int i = 0; i < 5; i++) {
      landmark_[i][0] = detection.landmark[i][0] * w;
      landmark_[i][1] = detection.landmark[i][1] * h;
    }
  }

  // QTimer::singleShot(25, this, SLOT(hide_self()));
  // show();
  QWidget *p = (QWidget *)parent();
  p->update();
}

void DetectTipWidget::hide_self() {
  // hide();
}

void DetectTipWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  painter.drawRect(rect_);
}
