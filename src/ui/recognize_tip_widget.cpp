#include "recognize_tip_widget.hpp"

#include <QDateTime>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <iostream>

#include "logger.hpp"

using namespace suanzi;

RecognizeTipWidget::RecognizeTipWidget(QWidget *parent) : QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  int wh = height() * 50 / 100;
  mask_ = QBitmap(wh, wh);
  QPainter painter(&mask_);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.fillRect(mask_.rect(), Qt::white);
  painter.setBrush(QColor(0, 0, 0));
  painter.drawRoundedRect(mask_.rect(), wh / 2, wh / 2);
}

RecognizeTipWidget::~RecognizeTipWidget() {}

void RecognizeTipWidget::rx_display(PersonData person) {
  person_ = person;

  hide();
  if (person_.status != "clear") show();
}

void RecognizeTipWidget::hide_self() { hide(); }

void RecognizeTipWidget::paintEvent(QPaintEvent *event) {
  if (person_.status != "clear") {
    QPainter painter(this);
    const int w = width();
    const int h = height();

    const QRect RECOGNIZE_TIP_BOX = {0, 0, w, h};

    // draw background
    painter.fillRect(RECOGNIZE_TIP_BOX, QColor(0, 0, 10, 150));

    // draw border and seperator
    painter.setPen(QPen(QColor(0, 0, 255, 128), 5));
    painter.drawRect(RECOGNIZE_TIP_BOX);
    painter.drawLine(w * 40 / 100, 50, w * 40 / 100, h - 50);

    // draw datetime
    QDateTime now = QDateTime::currentDateTime().toUTC().addSecs(8 * 3600);
    QString time = now.toString("hh : mm");
    QString date = now.toString("yyyy年MM月dd日");

    int base_font_size = h * 5 / 100;

    QFont font = painter.font();
    font.setPixelSize(base_font_size * 5);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(w * 8.25 / 100, h * 50 / 100, time);

    font.setPixelSize(base_font_size * 2);
    painter.setFont(font);
    painter.drawText(w * 6 / 100, h * 70 / 100, date);

    // draw avatar
    if (person_.face_path.length() > 0) {
      QPixmap avatar(person_.face_path.c_str());
      avatar = avatar.scaled(mask_.size());
      avatar.setMask(mask_);
      painter.drawPixmap(
          QRect(w * 50 / 100, h * 23.2 / 100, w * 18.75 / 100, h * 53.57 / 100),
          avatar, QRect());
    }

    // draw person info
    font.setPixelSize(base_font_size * 3);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(w * 72.5 / 100, h * 46.42 / 100, person_.name.c_str());

    if (person_.number.length() > 0) {
      font.setPixelSize(base_font_size * 2);
      painter.setFont(font);
      painter.drawText(w * 72.5 / 100, h * 71.43 / 100,
                       QString(("工号: " + person_.number).c_str()));
    }
  }
}
