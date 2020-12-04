#include "touch_widget.hpp"
#include <quface/logger.hpp>
#include <QVBoxLayout>
#include <QTimer>

using namespace suanzi;

TouchWidget::TouchWidget(int screen_width, int screen_height, QWidget *parent)
    : screen_width_(screen_width), screen_height_(screen_height), QWidget(parent) {

  setStyleSheet("QWidget {background-color:transparent;margin:0px;}");
  stacked_widget_ = new QStackedWidget(this);

  MenuKeyWidget *menu_key_widget = new MenuKeyWidget(screen_width, screen_height, this);
  stacked_widget_->addWidget(menu_key_widget);

  qrcode_widget_ = new QrcodeWidget(screen_width, screen_height, this);
  stacked_widget_->addWidget(qrcode_widget_);


  digit_key_widget_ = new DigitKeyWidget(screen_width, screen_height, this);
  stacked_widget_->addWidget(digit_key_widget_);

  connect(menu_key_widget, SIGNAL(switch_stacked_widget(int)), this, SLOT(switch_stacked_widget(int)));
  connect(qrcode_widget_, SIGNAL(switch_stacked_widget(int)), this, SLOT(switch_stacked_widget(int)));
  connect(digit_key_widget_, SIGNAL(switch_stacked_widget(int)), this, SLOT(switch_stacked_widget(int)));

  connect(menu_key_widget, SIGNAL(tx_menu_type(int)), digit_key_widget_, SLOT(rx_menu_type(int)));


  switch_stacked_widget(0);

}

TouchWidget::~TouchWidget() {

}


void TouchWidget::switch_stacked_widget(int index) {

	if (index == 0) {
		stacked_widget_->setStyleSheet("QWidget {background-color:rgba(5, 0, 20, 150);margin:0px;}");
		stacked_widget_->setFixedSize(screen_width_, 220);
		stacked_widget_->move(0, screen_height_ - 220);
	} else if (index == 1) {
		stacked_widget_->setStyleSheet("QWidget {background-color:transparent;margin:0px;color:white;}");
		stacked_widget_->setFixedSize(screen_width_, screen_height_);
		stacked_widget_->move(0, 0);
		qrcode_widget_->init();
	} else {
		stacked_widget_->setStyleSheet("QWidget {background-color:rgb(5, 0, 20);margin:0px;color:white;}");
		stacked_widget_->setFixedSize(screen_width_, screen_height_);
		stacked_widget_->move(0, 0);
		digit_key_widget_->init();
	}
	stacked_widget_->setCurrentIndex(index);
}
