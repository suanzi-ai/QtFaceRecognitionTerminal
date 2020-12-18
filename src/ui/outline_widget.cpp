#include "outline_widget.hpp"

#include <iostream>

#include <QDateTime>
#include <QPainter>
#include <QTimer>

#include "config.hpp"

using namespace suanzi;

OutlineWidget::OutlineWidget(int width, int height, QWidget *parent)
    : QWidget(parent), show_valid_rect_(false) {
  setAttribute(Qt::WA_StyledBackground, true);
  setAutoFillBackground(true);

  move(0, 0);
  setFixedSize(width, height);

  // set background image
  background_style_ =
      QString(
          "QWidget {color:red;background-image: url(:asserts/outline.png); "
          "background-repeat:no-repeat; margin-top: %1px; }")
          .arg(QString::number(0.1875 * height));

  no_style_ = QString(
                  "QWidget {color:red;background-image: none; "
                  "background-repeat:no-repeat; margin-top: %1px; }")
                  .arg(QString::number(0.1875 * height));

  rx_update();

  timer_.setInterval(1000);
  connect((const QObject *)&timer_, SIGNAL(timeout()), (const QObject *)this,
          SLOT(rx_update()));

  pl_temperature_ = new TemperatureTipLabel();
  pl_temperature_->setFixedSize(240, 165);
  pl_temperature_->move(width / 2 - 124, 0.24 * height);
  pl_temperature_->setStyleSheet(
      "QLabel { background-color:transparent; color:blue; }");
  pl_temperature_->hide();

  timer_.start();
}

OutlineWidget::~OutlineWidget() {}

void OutlineWidget::rx_temperature(float temp) {
  pl_temperature_->set_temperature(temp);
  pl_temperature_->show();
}

void OutlineWidget::rx_update() {
  static bool enable_temperature = false;
  if (enable_temperature != Config::get_user().enable_temperature) {
    enable_temperature = Config::get_user().enable_temperature;
    if (enable_temperature)
      setStyleSheet(background_style_);
    else
      setStyleSheet(no_style_);
  }
}

void OutlineWidget::rx_warn_distance() {}
