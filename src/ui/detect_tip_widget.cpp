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
  if (is_updated_) painter->drawRect(rect_);
}

void DetectTipWidget::rx_result(PingPangBuffer<ImagePackage> *img,
                                DetectionFloat detection) {
  int w = 800;
  int h = 1280;
  is_updated_ = detection.b_valid;
  if (is_updated_)
    this->rect_ = QRect(detection.x * w, detection.y * h, detection.width * w,
                        detection.height * h);

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
