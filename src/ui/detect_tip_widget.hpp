#ifndef DETECTTIPWIDGET_H
#define DETECTTIPWIDGET_H

#include <QImage>
#include <QPainter>
#include <QWidget>

#include "detection_float.h"
#include "image_package.h"
#include "pingpang_buffer.h"

namespace suanzi {

class DetectTipWidget : public QWidget {
  Q_OBJECT

 public:
  DetectTipWidget(QWidget *parent = nullptr);
  ~DetectTipWidget() override;

  void paint(QPainter *painter);

 private slots:
  void rx_display(DetectionRadio detection);

 private:
  static constexpr int MAX_RECT_COUNT = 10;
  static constexpr int MAX_LOST_AGE = 5;

  std::vector<QRect> rects_;

  bool is_updated_;
};

}  // namespace suanzi

#endif
