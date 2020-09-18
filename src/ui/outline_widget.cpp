#include "outline_widget.hpp"

#include <QPainter>

#include "config.hpp"

using namespace suanzi;

OutlineWidget::OutlineWidget(int width, int height, QWidget *parent)
    : QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);
}

OutlineWidget::~OutlineWidget() {}

void OutlineWidget::paint(QPainter *painter) {
  const int w = width();
  const int h = height();

  auto temperature = Config::get_temperature();

  float face_x = temperature.device_face_x * w;
  float face_y = temperature.device_face_y * h;
  float face_width = temperature.device_face_width * w;
  float face_height = temperature.device_face_height * h;
  float start_angle = temperature.device_body_start_angle;
  float open_angle = temperature.device_body_open_angle;

  painter->setPen(QPen(Qt::white, 5, Qt::SolidLine));
  painter->drawEllipse(face_x, face_y, face_width, face_height);

  painter->drawChord(face_x / 1.2,  // 1.2是身体要比头宽一点
                    face_y + face_height + 25,  // 25是身体与头部的间距
                    face_width * 1.2,  // 1.2是身体要比头宽一点
                    face_height, start_angle, open_angle);
}
