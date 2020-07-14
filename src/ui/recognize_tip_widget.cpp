#include "recognize_tip_widget.hpp"

#include <QDateTime>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>

#include <iostream>

#include "logger.hpp"

using namespace suanzi;

RecognizeTipWidget::RecognizeTipWidget(QWidget *parent)
    : QWidget(parent), mask_(150, 150) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  QPainter painter(&mask_);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.fillRect(mask_.rect(), Qt::white);
  painter.setBrush(QColor(0, 0, 0));
  painter.drawRoundedRect(mask_.rect(), 150 / 2, 150 / 2);
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
    const QRect RECOGNIZE_TIP_BOX = {0, 0, 800, 280};

    // draw background
    painter.fillRect(RECOGNIZE_TIP_BOX, QColor(0, 0, 10, 150));

    // draw border and seperator
    painter.setPen(QPen(QColor(0, 0, 255, 128), 5));
    painter.drawRect(RECOGNIZE_TIP_BOX);
    painter.drawLine(350, 50, 350, 230);

    // draw datetime
    QDateTime now = QDateTime::currentDateTime().toUTC().addSecs(8 * 3600);
    QString time = now.toString("hh : mm");
    QString date = now.toString("yyyy年MM月dd日");

    QFont font = painter.font();
    font.setPixelSize(70);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(70, 140, time);

    font.setPixelSize(35);
    painter.setFont(font);
    painter.drawText(48, 200, date);

    // draw avatar
    if (person_.face_path.length() > 0) {
      QPixmap avatar(person_.face_path.c_str());
      avatar = avatar.scaled(mask_.size());
      avatar.setMask(mask_);
      painter.drawPixmap(QRect(400, 65, 150, 150), avatar, QRect());
    }

    // draw person info
    font.setPixelSize(45);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(580, 130, person_.name.c_str());

    if (person_.number.length() > 0) {
      font.setPixelSize(30);
      painter.setFont(font);
      painter.drawText(580, 200, QString(("工号: " + person_.number).c_str()));
    }
  }
}