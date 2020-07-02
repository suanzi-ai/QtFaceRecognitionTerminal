#ifndef DETECTTIPWIDGET_H
#define DETECTTIPWIDGET_H

#include <QImage>
#include <QPainter>
#include <QWidget>

#include "detection_float.h"
#include "image_package.h"
#include "pingpangbuffer.h"

namespace suanzi {

class DetectTipWidget : public QWidget {
  Q_OBJECT

 public:
  DetectTipWidget(QWidget *parent = nullptr);
  ~DetectTipWidget() override;

  void paint(QPainter *painter);

 protected:
  void paintEvent(QPaintEvent *event) override;

 private slots:
  void rx_result(PingPangBuffer<ImagePackage> *img, DetectionFloat detection);
  void hide_self();

 private:
  QRect rect_;
  bool is_updated_;
};

}  // namespace suanzi

#endif
