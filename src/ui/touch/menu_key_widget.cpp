#include "menu_key_widget.hpp"
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

using namespace suanzi;

MenuKeyWidget::MenuKeyWidget(int width, int height, QWidget *parent)
    : QWidget(parent) {
  QPushButton *ppb_qrcode = new QPushButton(this);
  ppb_qrcode->setFixedSize(100, 100);
  ppb_qrcode->setStyleSheet(
      "QPushButton {border-image: url(:asserts/qrcode.png);}");
  ppb_qrcode->setFocusPolicy(Qt::NoFocus);
  QPushButton *ppb_intercom = new QPushButton(this);
  ppb_intercom->setFixedSize(100, 100);
  ppb_intercom->setStyleSheet(
      "QPushButton {border-image: url(:asserts/intercom.png);}");
  ppb_intercom->setFocusPolicy(Qt::NoFocus);

  QPushButton *ppb_password = new QPushButton(this);
  ppb_password->setFixedSize(100, 100);
  ppb_password->setStyleSheet(
      "QPushButton {border-image: url(:asserts/password.png);}");
  ppb_password->setFocusPolicy(Qt::NoFocus);

  QVBoxLayout *pv_layout = new QVBoxLayout;
  pv_layout->addStretch();
  pv_layout->addWidget(ppb_qrcode);
  pv_layout->addWidget(ppb_intercom);
  pv_layout->addWidget(ppb_password);
  pv_layout->addStretch();
  setLayout(pv_layout);

  connect(ppb_qrcode, SIGNAL(pressed()), this, SLOT(clicked_qrcode()));
  connect(ppb_intercom, SIGNAL(pressed()), this, SLOT(clicked_intercom()));
  connect(ppb_password, SIGNAL(pressed()), this, SLOT(clicked_password()));
}

MenuKeyWidget::~MenuKeyWidget() {}

void MenuKeyWidget::clicked_qrcode() { emit switch_stacked_widget(1); }

void MenuKeyWidget::clicked_intercom() {
  emit tx_menu_type(1);
  emit switch_stacked_widget(2);
}

void MenuKeyWidget::clicked_password() {
  emit tx_menu_type(2);
  emit switch_stacked_widget(2);
}
