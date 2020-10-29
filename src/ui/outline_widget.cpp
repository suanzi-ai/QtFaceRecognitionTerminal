#include "outline_widget.hpp"

#include <QPainter>

#include "config.hpp"

using namespace suanzi;

OutlineWidget::OutlineWidget(int width, int height, QWidget *parent)
    : QWidget(parent), outline_(":asserts/outline.png") {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);
}

OutlineWidget::~OutlineWidget() {}

void OutlineWidget::paint(QPainter *painter) {
  if (!Config::get_user().enable_temperature) return;

  int w = width();   // 800
  int h = height();  // 1280

  painter->drawPixmap(QRect(0, 0.1875 * h, w, 0.6171875 * h), outline_,
                      QRect());
}
