#include "face_box_widget.hpp"

#include <QDateTime>
#include <QPaintEvent>
#include <QPainter>

#include "logger.hpp"

using namespace suanzi;

FaceBoxWidget::FaceBoxWidget(int x, int y, int width, int height,
                             QWidget *parent)
    : x_(x), y_(y), width_(width), height_(height), QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);
}

FaceBoxWidget::~FaceBoxWidget() {}

void FaceBoxWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  const QRect FACE_BOX = {x_, y_, width_, height_};

  // draw background
  painter.fillRect(FACE_BOX, QColor(0, 0, 10, 150));

  // draw border and seperator
  painter.setPen(QPen(QColor(0, 0, 255, 128), 5));
  painter.drawRect(FACE_BOX);
}
