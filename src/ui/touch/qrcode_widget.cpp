#include "qrcode_widget.hpp"

#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QStackedWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <quface-io/engine.hpp>
#include <quface-io/ive.hpp>
#include <quface-io/mmzimage.hpp>
#include <quface/common.hpp>

#include "config.hpp"

//#include "JQQRCodeReader/JQQRCodeReader.h"
#include "zbar.h"

using namespace suanzi;
using namespace suanzi::io;
using namespace zbar;
QrcodeWidget::QrcodeWidget(int screen_width, int screen_height, QWidget *parent)
    : screen_width_(screen_width),
      screen_height_(screen_height),
      QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground, true);
  QPushButton *ppb_back = new QPushButton(this);
  ppb_back->setFixedSize(150, 150);
  ppb_back->setStyleSheet(
      "QPushButton {border-image: url(:asserts/back_small.png);}");
  ppb_back->setFocusPolicy(Qt::NoFocus);

  qrcode_scan_widget_ = new QrcodeScanWidget(QRCODE_WIDTH, QRCODE_HEIGHT, this);
  qrcode_scan_widget_->move((screen_width - QRCODE_WIDTH) / 2, 100);
  ppb_back->move((screen_width - 150) / 2, QRCODE_HEIGHT + 100 + 20);

  connect(ppb_back, SIGNAL(pressed()), this, SLOT(clicked_back()));

  qrcode_task_ = QrcodeTask::get_instance();
  connect(this, SIGNAL(start_qrcode_task()), (QObject *)qrcode_task_,
          SLOT(start_qrcode_task()));
  connect(this, SIGNAL(stop_qrcode_task()), (QObject *)qrcode_task_,
          SLOT(stop_qrcode_task()));
  connect((QObject *)qrcode_task_, SIGNAL(tx_ok()), (QObject *)this,
          SLOT(clicked_back()));

  hide();
}

QrcodeWidget::~QrcodeWidget() {}

void QrcodeWidget::init() {
  qrcode_scan_widget_->start_qrcode();
  emit start_qrcode_task();
  show();
}

void QrcodeWidget::clicked_back() {
  qrcode_scan_widget_->stop_qrcode();
  emit stop_qrcode_task();
  emit switch_stacked_widget(0);
}
