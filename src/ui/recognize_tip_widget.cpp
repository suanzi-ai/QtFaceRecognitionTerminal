#include "recognize_tip_widget.hpp"

#include <QDateTime>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <quface-io/engine.hpp>
#include <quface/logger.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

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

  timer_.setInterval(1000);
  timer_.setSingleShot(true);
  connect((const QObject *)&timer_, SIGNAL(timeout()), (const QObject *)this,
          SLOT(rx_reset()));
}

RecognizeTipWidget::~RecognizeTipWidget() {}

void RecognizeTipWidget::rx_display(PersonData person, bool if_duplicated) {
  person_ = person;

  hide();
  show();

  timer_.stop();
  timer_.start();
}

void RecognizeTipWidget::rx_reset() {
  hide();
}

void RecognizeTipWidget::paintEvent(QPaintEvent *event) {
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
  QDateTime now = QDateTime::currentDateTime();
  QString time = now.toString(tr("hh : mm"));
  QString date = now.toString(tr("yyyy年MM月dd日"));

  int base_font_size = h * 5 / 100;

  QFont font = painter.font();
  font.setPixelSize(base_font_size * 5);
  painter.setFont(font);
  painter.setPen(Qt::white);
  painter.drawText(w * 7 / 100, h * 50 / 100, time);

  font.setPixelSize(base_font_size * 2);
  painter.setFont(font);
  painter.drawText(w * 7 / 100, h * 70 / 100, date);

  // draw person info and avatar
  QString name, staff_number, avatar_path;
  name = person_.name.c_str();
  avatar_path = person_.face_path.c_str();
  if (person_.number.length() > 0)
    staff_number = (tr("工号: ").toStdString() + person_.number).c_str();
  else
    staff_number = "";

  QPixmap avatar(avatar_path);
  avatar = avatar.scaled(mask_.size());
  avatar.setMask(mask_);
  painter.drawPixmap(
      QRect(w * 50 / 100, h * 23.2 / 100, w * 18.75 / 100, h * 53.57 / 100),
      avatar, QRect());

  // draw person info
  font.setPixelSize(base_font_size * 3);
  painter.setFont(font);
  painter.setPen(Qt::white);
  painter.drawText(w * 72.5 / 100, h * 46.42 / 100, name);

  bool enable_temperature = Config::get_user().enable_temperature;
  std::stringstream ss;
  ss << tr("温度: ").toStdString() << std::setprecision(3) << person_.temperature << "°C";
  QString body_temperature = ss.str().c_str();

  font.setPixelSize(base_font_size * 2);
  painter.setFont(font);
  if (staff_number.length() > 0) {
    painter.drawText(w * 72.5 / 100, h * 71.43 / 100, staff_number);
    if (enable_temperature) {
      // draw temperature
      if (!person_.is_temperature_normal())
        painter.setPen(Qt::red);
      else
        painter.setPen(Qt::white);
      painter.drawText(w * 72.5 / 100, h * 96.44 / 100, body_temperature);
    }
  } else if (enable_temperature) {
    // draw temperature
    if (!person_.is_temperature_normal())
      painter.setPen(Qt::red);
    else
      painter.setPen(Qt::white);
    painter.drawText(w * 72.5 / 100, h * 71.43 / 100, body_temperature);
  }
}
