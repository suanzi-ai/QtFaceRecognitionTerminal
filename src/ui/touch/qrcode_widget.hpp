#ifndef QRCODE_KEY_WIDGET_H
#define QRCODE_KEY_WIDGET_H

#include <QLabel>
#include <QPainter>
#include <QPen>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <opencv2/opencv.hpp>
#include "person_service.hpp"
#include "qrcode_task.hpp"

namespace suanzi {

class QrcodeScanWidget : public QLabel {
  Q_OBJECT

 public:
  QrcodeScanWidget(int width, int height, QWidget *parent = nullptr)
      : QLabel(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(width, height);

    pl_lightneedle_ = new QLabel(this);
    pl_lightneedle_->setFixedSize(width - 10, 48);
    pl_lightneedle_->setStyleSheet(
        "QLabel {border-image: url(:asserts/lightneedle.png);}");
    pos_x_ = 2;
    pl_lightneedle_->hide();

    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(update_lightneedle_pos()));

    /*setStyleSheet(
    "QWidget "
    "{background-color:transparent;margin:0px;color:white;}");*/
  }

  void start_qrcode() {
    // timer_->start(30);
    pos_y_ = 2;
    // pl_lightneedle_->move(pos_x_, pos_y_);
    // pl_lightneedle_->show();
  }

  void stop_qrcode() { timer_->stop(); }

 private slots:
  void update_lightneedle_pos() {
    const int max_pos_y = height() - pl_lightneedle_->height() - 2;
    pos_y_ += 10;
    if (pos_y_ > max_pos_y) pos_y_ = 2;
    pl_lightneedle_->move(pos_x_, pos_y_);
  }

 private:
  void paintEvent(QPaintEvent *event) override {
    QPainter painter(this);
    QPen pen = painter.pen();
    pen.setWidth(5);
    pen.setBrush(QColor(2, 168, 223));
    painter.setPen(pen);

    float width = this->width();
    float height = this->height();
    // int x = (screen_width_ - QRCODE_WIDTH) / 2;
    // int y = 100;  //(screen_height_ - QRCODE_HEIGHT) / 2;
    float top_x = 0;
    float top_y = 0;
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

 private:
  int pos_x_;
  int pos_y_;
  QTimer *timer_;
  QLabel *pl_lightneedle_;
};

class QrcodeWidget : public QWidget {
  Q_OBJECT

 public:
  QrcodeWidget(int width, int height, QWidget *parent = nullptr);
  ~QrcodeWidget() override;
  void init();

 private:
  // void paintEvent(QPaintEvent *event) override;

 signals:
  void switch_stacked_widget(int index);
  void tx_display(PersonData person, bool if_duplicated);
  void start_qrcode_task();
  void stop_qrcode_task();

 private slots:
  void clicked_back();
  // void scan_qrcode_ui();

 private:
  QTimer *timer_;
  QLabel *pl_lightneedle_;
  int pos_x_;
  int pos_y_;
  int screen_width_;
  int screen_height_;
  const int QRCODE_WIDTH = 400;
  const int QRCODE_HEIGHT = 400;

  QrcodeTask *qrcode_task_;
  QrcodeScanWidget *qrcode_scan_widget_;
};

}  // namespace suanzi

#endif
