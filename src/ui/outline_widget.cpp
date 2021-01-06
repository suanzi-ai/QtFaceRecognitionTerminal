#include "outline_widget.hpp"
#include <iostream>
#include <quface-io/option.hpp>

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

  // background-position: 10% 40%;margin-top: %1px;
  // set background image
  int manufacturer = Config::get_temperature().manufacturer;
  int margin_top = height * 0.1875;
  if (width == 800) {
    if (manufacturer == TemperatureManufacturer::Dashu) {
      margin_top = height * 0.34375;
    }
    background_style_ =
        QString(
            "QWidget {color:red;background-image: url(:asserts/outline.png); "
            "background-repeat:no-repeat;margin-top: %1px;}")
            .arg(QString::number(margin_top));
  } else {
    margin_top = height * 0.165;
    background_style_ =
        QString(
            "QWidget {color:red;background-image: url(:asserts/outline.png); "
            "background-repeat:no-repeat;background-size: 61% "
            "61%;background-position: 10% 40%; margin-top: %1px;}")
            .arg(QString::number(margin_top));
  }
  setStyleSheet(background_style_);
  hide();

  rx_update();

  timer_.setInterval(1000);
  connect((const QObject *)&timer_, SIGNAL(timeout()), (const QObject *)this,
          SLOT(rx_update()));

  temp_tip_widget_ = new TemperatureTipWidget(width, height, margin_top);
  heatmap_widget_ = new HeatmapWidget(width, height);
  pl_temperature_ = nullptr;
  if (manufacturer == TemperatureManufacturer::Dashu)
    pl_temperature_ = new TemperatureTipLabel(width, height, margin_top);

  timer_.start();
}

OutlineWidget::~OutlineWidget() {}

void OutlineWidget::rx_temperature(float temp) {
  if (pl_temperature_ != nullptr) pl_temperature_->set_temperature(temp);
}

void OutlineWidget::rx_temperature(bool bvisible, bool bnormal_temperature,
                                   float temperature) {
  temp_tip_widget_->rx_temperature(bvisible, bnormal_temperature, temperature);
}

void OutlineWidget::rx_update() {
  static bool enable_temperature = false;
  if (enable_temperature != Config::display_temperature()) {
    enable_temperature = Config::display_temperature();
    if (enable_temperature)
      show();
    else
      hide();
  }
}

void OutlineWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  auto temperature = Config::get_temperature();
  if (temperature.temperature_area_debug) {
    float min_x = temperature.detect_area_x * width();
    float min_y = temperature.detect_area_y * height();
    float w = temperature.detect_area_width * width();
    float h = temperature.detect_area_height * height();

    // draw max temperature detection area
    painter.drawRect(min_x, min_y, w, h);

    // draw min temperature detection area
    float min_w = temperature.min_face_width * width();
    float min_h = temperature.min_face_height * height();
    painter.drawRect(min_x + (w - min_w) / 2, min_y + (h - min_h) / 2, min_w,
                     min_h);
  }
}

void OutlineWidget::rx_init(int success) { heatmap_widget_->rx_init(success); }

void OutlineWidget::rx_update(TemperatureMatrix mat, QRectF detection, float x,
                              float y) {
  heatmap_widget_->rx_update(mat, detection, x, y);
}

void OutlineWidget::rx_warn_distance() {}
