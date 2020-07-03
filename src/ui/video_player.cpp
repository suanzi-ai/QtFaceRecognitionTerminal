#include "video_player.hpp"

#include <QBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QTimer>

#include "videoframewidget.h"

using namespace suanzi;

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent) {
  QPalette pal = palette();
  pal.setColor(QPalette::Background, Qt::transparent);
  pal.setColor(QPalette::Foreground, Qt::green);
  setPalette(pal);

  detect_tip_widget_ = new DetectTipWidget(this);
  detect_tip_widget_->hide();
  // recognize_tip_widget_ = new RecognizeTipWidget();
  // recognize_tip_widget_->hide();

  // RGB Camera
  camera_reader_1_ = new CameraReader(1, this);
  // IR Camera
  // camera_reader_0_ = new CameraReader(0, this);

  detect_task_ = new DetectTask(nullptr, this);
  recognize_task_ = new RecognzieTask(nullptr, this);

  connect((const QObject *)camera_reader_1_,
          SIGNAL(tx_frame(PingPangBuffer<ImagePackage> *)),
          (const QObject *)detect_task_,
          SLOT(rx_frame(PingPangBuffer<ImagePackage> *)));

  connect((const QObject *)detect_task_, SIGNAL(tx_finish()),
          (const QObject *)camera_reader_1_, SLOT(rx_finish()));

  connect((const QObject *)detect_task_, SIGNAL(tx_display(DetectionFloat)),
          (const QObject *)detect_tip_widget_, SLOT(rx_display(DetectionFloat)));

  connect(
      (const QObject *)detect_task_,
      SIGNAL(tx_recognize(PingPangBuffer<ImagePackage> *, DetectionFloat)),
      (const QObject *)recognize_task_,
      SLOT(rx_frame(PingPangBuffer<ImagePackage> *, DetectionFloat)));

  connect((const QObject *)recognize_task_, SIGNAL(tx_finish()),
          (const QObject *)detect_task_, SLOT(rx_finish()));

  //   connect((const QObject *)recognize_task_, SIGNAL(txResult(Person)),
  //           (const QObject *)recognize_tip_widget_, SLOT(rxResult(Person)));

  QTimer::singleShot(1, this, SLOT(init_widgets()));
}

VideoPlayer::~VideoPlayer() {}

void VideoPlayer::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  detect_tip_widget_->paint(&painter);
  // painter.drawRect(RECOGNIZE_BOX);
}

void VideoPlayer::init_widgets() {
  recognize_tip_widget_->setFixedSize(RECOGNIZE_TIP_BOX.width(),
                                      RECOGNIZE_TIP_BOX.height());
  recognize_tip_widget_->move(RECOGNIZE_TIP_BOX.x(), RECOGNIZE_TIP_BOX.y());
  recognize_tip_widget_->show();
  /*detect_tip_widget_->setFixedSize(RECOGNIZE_BOX.width(),
  RECOGNIZE_BOX.height()); detect_tip_widget_->move(RECOGNIZE_BOX.x(),
  RECOGNIZE_BOX.y()); detect_tip_widget_->show();*/
}
