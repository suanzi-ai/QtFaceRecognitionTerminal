#include "qrcode_widget.hpp"
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>


using namespace suanzi;

QrcodeWidget::QrcodeWidget(int screen_width, int screen_height, QWidget *parent)
    : QWidget(parent) {

  QPushButton *ppb_back = new QPushButton(this);
  ppb_back->setFixedSize(150, 150);
  ppb_back->setStyleSheet("QPushButton {border-image: url(:asserts/back.png);}");
  ppb_back->setFocusPolicy(Qt::NoFocus);

  QVBoxLayout *pv_layout = new QVBoxLayout;
  pv_layout->addSpacing(560);
  pv_layout->addWidget(ppb_back, 0, Qt::AlignCenter);
  setLayout(pv_layout);

  connect(ppb_back, SIGNAL(pressed()), this, SLOT(clicked_back()));
}

QrcodeWidget::~QrcodeWidget() {}


void QrcodeWidget::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	QPen pen = painter.pen();
	pen.setWidth(3);
	painter.setPen(pen);

	float width = 400;
	float height = 400;
	int x = (this->width() - width) / 2;
  	int y = (this->height() - height) / 2;
	float top_x = x;
	float top_y = y;
	float bottom_x = top_x + width;
	float bottom_y = top_y + height;

	// SZ_LOG_INFO("top_x={},top_y={},bottom_x={},bottom_y={},w={},h={}", top_x,
	//              top_y, bottom_x, bottom_y, width, height);

	painter.drawLine(top_x, top_y, top_x + width / 5, top_y);
	painter.drawLine(top_x, top_y, top_x, top_y + height / 5);
	painter.drawLine(top_x, bottom_y, top_x + width / 5, bottom_y);
	painter.drawLine(top_x, bottom_y, top_x, bottom_y - height / 5);
	painter.drawLine(bottom_x, top_y, bottom_x, top_y + height / 5);
	painter.drawLine(bottom_x, top_y, bottom_x - width / 5, top_y);
	painter.drawLine(bottom_x, bottom_y, bottom_x, bottom_y - height / 5);
	painter.drawLine(bottom_x, bottom_y, bottom_x - width / 5, bottom_y);
}


void QrcodeWidget::clicked_back() {
	emit switch_stacked_widget(0);
}
