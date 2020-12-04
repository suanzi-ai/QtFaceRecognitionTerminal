#include "menu_key_widget.hpp"
#include <QPushButton>
#include <QHBoxLayout>
#include <QStackedWidget>

using namespace suanzi;

MenuKeyWidget::MenuKeyWidget(int width, int height, QWidget *parent)
    : QWidget(parent) {

    QPushButton *ppb_qrcode = new QPushButton(this);
	ppb_qrcode->setFixedSize(150, 150);
    ppb_qrcode->setStyleSheet("QPushButton {border-image: url(:asserts/qrcode.png);}");
	ppb_qrcode->setFocusPolicy(Qt::NoFocus);
	QPushButton *ppb_intercom = new QPushButton(this);
	ppb_intercom->setFixedSize(150, 150);
	ppb_intercom->setStyleSheet("QPushButton {border-image: url(:asserts/intercom.png);}");
	ppb_intercom->setFocusPolicy(Qt::NoFocus);

	QPushButton *ppb_password = new QPushButton(this);
	ppb_password->setFixedSize(150, 150);
	ppb_password->setStyleSheet("QPushButton {border-image: url(:asserts/password.png);}");
	ppb_password->setFocusPolicy(Qt::NoFocus);

	QHBoxLayout *ph_layout = new QHBoxLayout;
	ph_layout->addStretch();
	ph_layout->addWidget(ppb_qrcode);
	ph_layout->addWidget(ppb_intercom);
	ph_layout->addWidget(ppb_password);
	ph_layout->addStretch();
	setLayout(ph_layout);

	connect(ppb_qrcode, SIGNAL(pressed()), this, SLOT(clicked_qrcode()));
	connect(ppb_intercom, SIGNAL(pressed()), this, SLOT(clicked_intercom()));
	connect(ppb_password, SIGNAL(pressed()), this, SLOT(clicked_password()));


}

MenuKeyWidget::~MenuKeyWidget() {}


void MenuKeyWidget::clicked_qrcode() {
	emit switch_stacked_widget(1);
}


void MenuKeyWidget::clicked_intercom() {
	emit tx_menu_type(1);
	emit switch_stacked_widget(2);
}


void MenuKeyWidget::clicked_password() {
	emit tx_menu_type(2);
	emit switch_stacked_widget(2);
}
