#ifndef FACE_BOX_WIDGET_H
#define FACE_BOX_WIDGET_H

#include <QPixmap>
#include <QWidget>

namespace suanzi {

class FaceBoxWidget : public QWidget {
  Q_OBJECT

 public:
  FaceBoxWidget(int x, int y, int width, int height, QWidget *parent = nullptr);
  ~FaceBoxWidget() override;

 protected:
  void paintEvent(QPaintEvent *event);

 private:
  int x_, y_, width_, height_;
};

}  // namespace suanzi

#endif
