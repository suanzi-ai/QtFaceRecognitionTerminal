#ifndef DETECTTIPWIDGET_H
#define DETECTTIPWIDGET_H

#include <QImage>
#include <QPainter>
#include <QThread>
#include <QVBoxLayout>
#include <QWidget>

#include "detection_data.hpp"
#include "image_package.hpp"
#include "pingpang_buffer.hpp"

namespace suanzi {

class DetectTipWidget : public QWidget {
  Q_OBJECT

 public:
  DetectTipWidget(int win_x, int win_y, int win_width, int win_height,
                  QWidget *parent = nullptr);
  ~DetectTipWidget() override;

 private:
  void paint(QPainter *painter);
  void paintEvent(QPaintEvent *event) override;

 private slots:
  void rx_display(DetectionRatio detection, bool to_clear, bool valid,
                  bool is_bgr);

 private:
  void get_detect_position(float &x, float &y, float &width, float &height);

 private:
  class InternalEventLoopThread : public QThread {
   public:
    InternalEventLoopThread() { start(); }

   private:
    void run() { exec(); }
  };

 private:
  static constexpr int MAX_RECT_COUNT = 10;
  static constexpr int MAX_LOST_AGE = 5;

  std::vector<QRect> rects_;
  QPolygon landmarks_;
  QString pose_;

  int win_x_;
  int win_y_;
  int win_width_;
  int win_height_;

  int lost_age_;

  bool is_valid_;

  float detect_pos_x_;
  float detect_pos_y_;
  float detect_width_;
  float detect_height_;
};

}  // namespace suanzi

#endif
