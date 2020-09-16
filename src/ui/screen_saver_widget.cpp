#include "screen_saver_widget.hpp"

#include <QDateTime>
#include <QPaintEvent>
#include <QPainter>

#include <quface/logger.hpp>

using namespace suanzi;

ScreenSaverWidget::ScreenSaverWidget(int width, int height, QWidget *parent)
    : QWidget(parent), background_(":asserts/background.jpg") {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);
  background_.scaled(width, height);
}

ScreenSaverWidget::~ScreenSaverWidget() {}

void ScreenSaverWidget::rx_display(int disappear_seconds) {
  if (disappear_seconds > 60) {
    hide();
    show();
  }
}

void ScreenSaverWidget::rx_hide() { hide(); }

void ScreenSaverWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  const int w = width();
  const int h = height();

  // draw background
  //painter.drawPixmap(QRect(0, 0, w, h), background_, QRect());

  QPixmap pixmapToShow = QPixmap::fromImage(background_);
  painter.drawPixmap(QRect(0, 0, w, h), pixmapToShow, QRect());


  

  // draw datetime
  QDateTime now = QDateTime::currentDateTime().toUTC().addSecs(8 * 3600);

  QString time = now.toString(tr("hh   mm"));
  QString date = now.toString(tr("yyyy年MM月dd日"));

  int base_font_size = h * 2 / 100;

  QFont font = painter.font();
  font.setPixelSize(base_font_size * 5);
  painter.setFont(font);
  painter.setPen(Qt::white);
  painter.drawText(w * 25 / 100, h * 33 / 100, time);

  bool flag = (now.currentMSecsSinceEpoch() / 500) % 2;
  if (flag)
    painter.drawText(w * 47 / 100, h * 33 / 100, ":");

  font.setPixelSize(base_font_size * 2);
  painter.setFont(font);
  painter.drawText(w * 26 / 100, h * 40 / 100, date);
}
