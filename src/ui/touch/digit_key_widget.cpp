#include "digit_key_widget.hpp"
#include <QGridLayout>
#include <QStackedWidget>

using namespace suanzi;

DigitKeyWidget::DigitKeyWidget(int screen_width, int screen_height,
                               QWidget *parent)
    : screen_width_(screen_width),
      screen_height_(screen_height),
      QWidget(parent) {
  setStyleSheet(
      "QPushButton {border-image: "
      "url(:asserts/"
      "key_background.png);color:white;font-size:80px;padding-top:35px;}");

  QPushButton *ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setStyleSheet(
      "QPushButton {border-image: url(:asserts/back_small.png);margin:0px;}");
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_back()));
  QHBoxLayout *ph_layout = new QHBoxLayout;
  ph_layout->setSpacing(0);
  ph_layout->setMargin(0);
  ph_layout->addWidget(ppb_btn, 0, Qt::AlignLeft);

  ple_input_tip_ = new QLineEdit(this);
  ple_input_tip_->setFixedHeight(150);
  ple_input_tip_->setReadOnly(true);
  ple_input_tip_->setEchoMode(QLineEdit::Normal);
  ple_input_tip_->setStyleSheet(
      "QLineEdit {background-color:rgb(51, 51, "
      "51);color:white;font-size:80px;padding-top:35px;}");
  ph_layout->addWidget(ple_input_tip_);

  QVBoxLayout *pv_layout = new QVBoxLayout;
  pv_layout->addLayout(ph_layout);
  pv_layout->addSpacing(100);

  ph_layout = new QHBoxLayout;
  ph_layout->addStretch();

  QGridLayout *pg_layout = new QGridLayout;
  pg_layout->setSpacing(10);
  // row 1
  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("1");
  pg_layout->addWidget(ppb_btn, 0, 0);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_1_key()));

  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("2");
  pg_layout->addWidget(ppb_btn, 0, 1);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_2_key()));

  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("3");
  pg_layout->addWidget(ppb_btn, 0, 2);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_3_key()));

  // row 2
  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("4");
  pg_layout->addWidget(ppb_btn, 1, 0);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_4_key()));

  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("5");
  pg_layout->addWidget(ppb_btn, 1, 1);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_5_key()));

  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("6");
  pg_layout->addWidget(ppb_btn, 1, 2);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_6_key()));

  // row 3
  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("7");
  pg_layout->addWidget(ppb_btn, 2, 0);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_7_key()));

  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("8");
  pg_layout->addWidget(ppb_btn, 2, 1);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_8_key()));

  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("9");
  pg_layout->addWidget(ppb_btn, 2, 2);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_9_key()));

  // row 4
  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("#");
  pg_layout->addWidget(ppb_btn, 3, 0);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_well_key()));

  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setText("0");
  pg_layout->addWidget(ppb_btn, 3, 1);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_0_key()));

  ppb_btn = new QPushButton(this);
  setCommonAttribute(ppb_btn);
  ppb_btn->setStyleSheet(
      "QPushButton {border-image: url(:asserts/backspace.png);}");
  pg_layout->addWidget(ppb_btn, 3, 2);
  connect(ppb_btn, SIGNAL(pressed()), this, SLOT(clicked_backspace_key()));

  // row5
  ppb_doorbell_ = new QPushButton(this);
  setCommonAttribute(ppb_doorbell_);
  ppb_doorbell_->setStyleSheet(
      "QPushButton {border-image: url(:asserts/doorbell.png);}");
  pg_layout->addWidget(ppb_doorbell_, 4, 0);
  connect(ppb_doorbell_, SIGNAL(pressed()), this, SLOT(clicked_doorbell_key()));

  ppb_intercom_ = new QPushButton(this);
  setCommonAttribute(ppb_intercom_);
  ppb_intercom_->setStyleSheet(
      "QPushButton {border-image: url(:asserts/intercom_small.png);}");
  pg_layout->addWidget(ppb_intercom_, 4, 1);
  connect(ppb_intercom_, SIGNAL(pressed()), this, SLOT(clicked_intercom_key()));

  ppb_ok_ = new QPushButton(this);
  setCommonAttribute(ppb_ok_);
  ppb_ok_->setText("OK");
  ppb_ok_->hide();
  pg_layout->addWidget(ppb_ok_, 4, 1);
  connect(ppb_ok_, SIGNAL(pressed()), this, SLOT(clicked_ok_key()));

  ppb_video_ = new QPushButton(this);
  setCommonAttribute(ppb_video_);
  ppb_video_->setStyleSheet(
      "QPushButton {border-image: url(:asserts/video.png);}");
  pg_layout->addWidget(ppb_video_, 4, 2);
  connect(ppb_video_, SIGNAL(pressed()), this, SLOT(clicked_video_key()));

  ph_layout->addLayout(pg_layout);
  ph_layout->addStretch();

  pv_layout->addLayout(ph_layout);
  pv_layout->addStretch();

  setLayout(pv_layout);

  // rx_menu_type(2);
}

DigitKeyWidget::~DigitKeyWidget() {}

void DigitKeyWidget::init() { ple_input_tip_->clear(); }

void DigitKeyWidget::setCommonAttribute(QPushButton *ppb) {
  ppb->setFixedSize(150, 150);
  ppb->setFocusPolicy(Qt::NoFocus);
}

void DigitKeyWidget::clicked_back() { emit switch_stacked_widget(0); }

void DigitKeyWidget::clicked_0_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "0");
}

void DigitKeyWidget::clicked_1_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "1");
}

void DigitKeyWidget::clicked_2_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "2");
}

void DigitKeyWidget::clicked_3_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "3");
}

void DigitKeyWidget::clicked_4_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "4");
}

void DigitKeyWidget::clicked_5_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "5");
}

void DigitKeyWidget::clicked_6_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "6");
}

void DigitKeyWidget::clicked_7_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "7");
}

void DigitKeyWidget::clicked_8_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "8");
}

void DigitKeyWidget::clicked_9_key() {
  ple_input_tip_->setText(ple_input_tip_->text() + "9");
}

void DigitKeyWidget::clicked_well_key() {}

void DigitKeyWidget::clicked_backspace_key() { ple_input_tip_->backspace(); }

void DigitKeyWidget::clicked_doorbell_key() {}

void DigitKeyWidget::clicked_intercom_key() {}

void DigitKeyWidget::clicked_video_key() {}

void DigitKeyWidget::clicked_ok_key() {
  if (ple_input_tip_->text() == "123456") {
    printf("password ok\n");
    // emit tx_display(person_data, person_data.is_duplicated);
  }
  emit switch_stacked_widget(0);
}

void DigitKeyWidget::rx_menu_type(int menu_type) {
  if (menu_type == 1) {
    ppb_doorbell_->show();
    ppb_intercom_->show();
    ppb_video_->show();
    ppb_ok_->hide();
    ple_input_tip_->setEchoMode(QLineEdit::Normal);
  } else {
    ppb_doorbell_->hide();
    ppb_intercom_->hide();
    ppb_video_->hide();
    ppb_ok_->show();
    ple_input_tip_->setEchoMode(QLineEdit::Password);
  }
}
