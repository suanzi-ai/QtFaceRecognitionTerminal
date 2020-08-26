#include "recognize_tip_widget.hpp"

#include <QDateTime>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#include "config.hpp"
#include "gpio.hpp"
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

  timer_.setInterval(1000);
  timer_.setSingleShot(true);
  connect((const QObject *)&timer_, SIGNAL(timeout()), (const QObject *)this,
          SLOT(rx_reset()));
}

RecognizeTipWidget::~RecognizeTipWidget() {}

void RecognizeTipWidget::rx_display(PersonData person, bool if_duplicated) {
  person_ = person;

  // Open door GPIO
  if (person_.status == PersonService::get_status(PersonStatus::Normal))
    Gpio::set_level(GpioPinDOOR, true);

  hide();
  show();

  timer_.stop();
  timer_.start();
}

void RecognizeTipWidget::rx_reset() {
  hide();
  Gpio::set_level(GpioPinDOOR, false);
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

  // draw info and avatar
  QString name, staff_number, avatar_path;
  name = person_.name.c_str();
  avatar_path = person_.face_path.c_str();
  if (person_.number.length() > 0)
    staff_number = ("工号: " + person_.number).c_str();
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

  bool disable_temperature = Config::get_app().disabled_temperature;
  if (staff_number.length() > 0) {
    font.setPixelSize(base_font_size * 2);
    painter.setFont(font);
    painter.drawText(w * 72.5 / 100, h * 71.43 / 100, staff_number);
    if (!disable_temperature) {
      QString body_temperature =
          ("温度: " + std::to_string(person_.temperature)).c_str();
      painter.drawText(w * 72.5 / 100, h * 96.44 / 100, body_temperature);
    }
  } else if (!disable_temperature) {
    QString body_temperature =
        ("温度: " + std::to_string(person_.temperature)).c_str();
    painter.drawText(w * 72.5 / 100, h * 71.43 / 100, body_temperature);
  }
}
