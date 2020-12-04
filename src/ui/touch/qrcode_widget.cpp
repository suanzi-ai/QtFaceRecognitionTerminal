#include "qrcode_widget.hpp"
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTimer>

using namespace suanzi;

QrcodeWidget::QrcodeWidget(int screen_width, int screen_height, QWidget *parent)
    : QWidget(parent) {

  QPushButton *ppb_back = new QPushButton(this);
  ppb_back->setFixedSize(150, 150);
  ppb_back->setStyleSheet("QPushButton {border-image: url(:asserts/back_small.png);}");
  ppb_back->setFocusPolicy(Qt::NoFocus);

  QVBoxLayout *pv_layout = new QVBoxLayout;
  pv_layout->addSpacing(QRCODE_HEIGHT + 170);
  pv_layout->addWidget(ppb_back, 0, Qt::AlignCenter);
  pv_layout->setMargin(0);
  setLayout(pv_layout);

  connect(ppb_back, SIGNAL(pressed()), this, SLOT(clicked_back()));

  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(scan_qrcode()));


  pl_lightneedle_ = new QLabel(this);
  pl_lightneedle_->setFixedSize(QRCODE_WIDTH - 10, 48);
  pl_lightneedle_->setStyleSheet("QLabel {border-image: url(:asserts/lightneedle.png);}");
  pos_x_ = (screen_width - QRCODE_WIDTH) / 2 + 2;
  pl_lightneedle_->hide();
}

QrcodeWidget::~QrcodeWidget() {}


void QrcodeWidget::init() {
	//adjustSize();
	timer_->start(50);
  	pos_y_ = (1280 - QRCODE_HEIGHT) / 2 + 2;
	pl_lightneedle_->move(pos_x_, pos_y_);
	pl_lightneedle_->show();
}


void QrcodeWidget::scan_qrcode() {
	const int max_pos_y = (this->height() - QRCODE_HEIGHT) / 2 + QRCODE_HEIGHT - pl_lightneedle_->height() - 2;
	pos_y_ += 10;
	if (pos_y_ > max_pos_y)
		pos_y_ = (this->height() - QRCODE_HEIGHT) / 2 + 2;
	pl_lightneedle_->move(pos_x_, pos_y_);
}


void QrcodeWidget::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	QPen pen = painter.pen();
	pen.setWidth(5);
	pen.setBrush(QColor(2, 168, 223));
	painter.setPen(pen);

	float width = QRCODE_WIDTH;
	float height = QRCODE_HEIGHT;
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
	timer_->stop();
	emit switch_stacked_widget(0);
}
