#include "recognize_tip_widget.hpp"

#include <QBitmap>
#include <QDateTime>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>

#include <iostream>

using namespace suanzi;

RecognizeTipWidget::RecognizeTipWidget(QWidget *parent) : QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);
}

RecognizeTipWidget::~RecognizeTipWidget() {}

void RecognizeTipWidget::rx_display(PersonDisplay person) {
  person_ = person;

  hide();
  if (!person_.to_clear) show();
}

void RecognizeTipWidget::hide_self() { hide(); }

void RecognizeTipWidget::paintEvent(QPaintEvent *event) {
  if (!person_.to_clear) {
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
    QPixmap avatar(person_.avatar_path.c_str());
    if (person_.avatar_path.length() > 0)
      painter.drawPixmap(
          QRect(400, 65, 150, 150),
          rectangle_to_round(avatar), QRect());

    // draw person info
    font.setPixelSize(45);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(580, 130, person_.name.c_str());

    if (person_.id.length() > 0) {
      font.setPixelSize(30);
      painter.setFont(font);
      painter.drawText(580, 200, QString(("ID: " + person_.id).c_str()));
    }
  }
}

QPixmap RecognizeTipWidget::rectangle_to_round(QPixmap &input_image) {
  if (input_image.isNull()) {
    return QPixmap();
  }
  QSize size(input_image.size());
  QBitmap mask(input_image.size());
  QPainter painter(&mask);
  painter.setRenderHint(QPainter::Antialiasing);
  painter.setRenderHint(QPainter::SmoothPixmapTransform);
  painter.fillRect(mask.rect(), Qt::white);
  painter.setBrush(QColor(0, 0, 0));
  painter.drawRoundedRect(mask.rect(), size.width() / 2, size.height() / 2);
  QPixmap image = input_image;
  image.setMask(mask);
  return image;
}