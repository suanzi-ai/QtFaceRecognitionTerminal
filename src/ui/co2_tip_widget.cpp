#include "co2_tip_widget.hpp"
#include <QPainter>
#include "config.hpp"

using namespace suanzi;

Co2TipWidget::Co2TipWidget(int screen_width, int screen_height, QWidget *parent)
    : cur_level_(-1), cur_ppm_value_(0), QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground, true);
  setStyleSheet("QWidget {border-image: url(:asserts/co2/co2_level1.png);}");
  const int w = screen_width;
  const int h = screen_height;
  move(w - 220, 0.84375 * h - 88);
  setFixedSize(220, 88);

  hide();
  QTimer::singleShot(1, this, SLOT(delay_init_widgets()));
}

Co2TipWidget::~Co2TipWidget() {}

void Co2TipWidget::delay_init_widgets() { show(); }

void Co2TipWidget::rx_co2(int value) {
  cur_ppm_value_ = value;

  int level = 0;
  if (value < 450) {
    level = 1;
  } else if (value >= 450 && value < 700) {
    level = 2;
  } else if (value >= 700 && value < 1000) {
    level = 3;
  } else if (value >= 1000 && value < 2500) {
    level = 4;
  } else if (value >= 2500 && value < 5000) {
    level = 5;
  } else {
    level = 6;
  }

  if (cur_level_ != level) {
    cur_level_ = level;
    setStyleSheet(
        QString("QWidget {border-image: url(:asserts/co2/co2_level%1.png);}")
            .arg(QString::number(level)));
  }
}

void Co2TipWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  QFont font = painter.font();
  font.setPixelSize(30);
  font.setBold(true);
  painter.setFont(font);
  painter.setPen(Qt::blue);
  painter.drawText(80, 54, QString::number(cur_ppm_value_) + "ppm");
}
