#ifndef QRCODE_KEY_WIDGET_H
#define QRCODE_KEY_WIDGET_H

#include <QLabel>
#include <QThread>
#include <QTimer>
#include <QWidget>
#include <opencv2/opencv.hpp>
#include "person_service.hpp"

namespace suanzi {

class QrcodeWidget : public QWidget {
  Q_OBJECT

 public:
  QrcodeWidget(int width, int height, QWidget *parent = nullptr);
  ~QrcodeWidget() override;
  void init();

 private:
  void paintEvent(QPaintEvent *event) override;
  std::string zbar_decoder(cv::Mat &img);
  QString get_qrcode(cv::Mat &img);
  std::string get_qrcode_in_binimg(cv::Mat &bin_img);
  void scan_qrcode();

 signals:
  void switch_stacked_widget(int index);
  void tx_display(PersonData person, bool if_duplicated);

 private slots:
  void clicked_back();
  void scan_qrcode_ui();

 private:
  class ScanQrcodeTask : public QThread {
   public:
    ScanQrcodeTask(QObject *parent);
    void run() override;
  };

 private:
  friend class ScanQrcodeTask;
  bool bstart_scan_qrcode_;
  QTimer *timer_;
  QLabel *pl_lightneedle_;
  int pos_x_;
  int pos_y_;
  int screen_width_;
  int screen_height_;
  const int QRCODE_WIDTH = 400;
  const int QRCODE_HEIGHT = 400;
};

}  // namespace suanzi

#endif
