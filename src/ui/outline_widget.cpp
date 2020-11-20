#include "outline_widget.hpp"

#include <iostream>

#include <QDateTime>
#include <QPainter>
#include <QTimer>

#include "config.hpp"

using namespace suanzi;

OutlineWidget::OutlineWidget(int width, int height, QWidget *parent)
    : QWidget(parent),
      show_valid_rect_(false) {
	QPalette palette = this->palette();
	palette.setColor(QPalette::Background, Qt::transparent);
	setPalette(palette);
	setAttribute(Qt::WA_StyledBackground, true);
	setAutoFillBackground(true);

	move(0, 0);
	setFixedSize(width, height);

	//set background image
	QString style_str = "QWidget {background-image: url(:asserts/outline.png);";
	style_str += "background-repeat:no-repeat;margin-top: ";
	style_str += QString::number(0.1875 * height) + "px;}";
	setStyleSheet(style_str);

	timer_.setInterval(1000);
	timer_.setSingleShot(true);
	connect((const QObject *)&timer_, SIGNAL(timeout()), (const QObject *)this,
	      SLOT(rx_reset()));
}

OutlineWidget::~OutlineWidget() {}

void OutlineWidget::paint(QPainter *painter) {
  if (!Config::get_user().enable_temperature) return;

  int w = width();   // 800
  int h = height();  // 1280
  if (show_valid_rect_) {
    QDateTime now = QDateTime::currentDateTime();
    bool flag = (now.currentMSecsSinceEpoch() / 250) % 2;
    if (flag) {
      auto cfg = Config::get_temperature();
      float top_x = cfg.device_face_x * w;
      float top_y = cfg.device_face_y * h;
      float bottom_x = top_x + cfg.device_face_width * w;
      float bottom_y = top_y + cfg.device_face_height * h;
      float length = 0.06 * w;

      painter->setPen(QPen(QColor(255, 0, 0, 200), 2));
      painter->drawLine(top_x, top_y, top_x + length, top_y);
      painter->drawLine(top_x, top_y, top_x, top_y + length);
      painter->drawLine(top_x, bottom_y, top_x + length, bottom_y);
      painter->drawLine(top_x, bottom_y, top_x, bottom_y - length);
      painter->drawLine(bottom_x, top_y, bottom_x, top_y + length);
      painter->drawLine(bottom_x, top_y, bottom_x - length, top_y);
      painter->drawLine(bottom_x, bottom_y, bottom_x, bottom_y - length);
      painter->drawLine(bottom_x, bottom_y, bottom_x - length, bottom_y);
    }
  }
}

void OutlineWidget::rx_warn_distance() {
  show_valid_rect_ = true;
  timer_.stop();
  timer_.start();
}

void OutlineWidget::rx_reset() { show_valid_rect_ = false; }
