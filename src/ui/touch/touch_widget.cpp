#include "touch_widget.hpp"
#include <QTimer>
#include <QVBoxLayout>
#include <quface/logger.hpp>

using namespace suanzi;

TouchWidget::TouchWidget(int screen_width, int screen_height, QWidget *parent)
    : screen_width_(screen_width),
      screen_height_(screen_height),
      parent_widget_(parent) /*, QStackedWidget(parent)*/ {
  // setStyleSheet("QWidget {background-color:transparent;margin:0px;}");
  // stacked_widget_ = new QStackedWidget(this);

  menu_key_widget_ = new MenuKeyWidget(screen_width, screen_height, this);
  addWidget(menu_key_widget_);

  qrcode_widget_ = new QrcodeWidget(screen_width, screen_height, this);
  addWidget(qrcode_widget_);

  digit_key_widget_ = new DigitKeyWidget(screen_width, screen_height, this);
  addWidget(digit_key_widget_);

  connect(menu_key_widget_, SIGNAL(switch_stacked_widget(int)), this,
          SLOT(switch_stacked_widget(int)));
  connect(qrcode_widget_, SIGNAL(switch_stacked_widget(int)), this,
          SLOT(switch_stacked_widget(int)));
  connect(digit_key_widget_, SIGNAL(switch_stacked_widget(int)), this,
          SLOT(switch_stacked_widget(int)));

  connect(menu_key_widget_, SIGNAL(tx_menu_type(int)), digit_key_widget_,
          SLOT(rx_menu_type(int)));

  switch_stacked_widget(0);
}

TouchWidget::~TouchWidget() {}

void TouchWidget::switch_stacked_widget(int index) {
  if (index == 0) {
    setStyleSheet("QStackedWidget {background-color:transparent;margin:0px;}");
    int w = menu_key_widget_->width();
    int h = menu_key_widget_->height();
    setFixedSize(100, 300);
    move(screen_width_ - 100, (screen_height_ - 350) / 2);
  } else if (index == 1) {
    setStyleSheet(
        "QStackedWidget "
        "{background-color:transparent;margin:0px;color:white;}");
    setFixedSize(screen_width_, screen_height_);
    move(0, 0);
    qrcode_widget_->init();
  } else {
    setStyleSheet(
        "QStackedWidget {background-color:rgb(5, 0, "
        "20);margin:0px;color:white;}");
    setFixedSize(screen_width_, screen_height_);
    move(0, 0);
    digit_key_widget_->init();
  }
  setCurrentIndex(index);
  parent_widget_->repaint();
}

void TouchWidget::rx_display(bool invisible) {
  if (invisible)
    hide();
  else
    show();
}
