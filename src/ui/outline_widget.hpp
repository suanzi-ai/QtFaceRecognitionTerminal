#ifndef OUTLINE_WIDGET_H
#define OUTLINE_WIDGET_H

#include <QWidget>

namespace suanzi {

class OutlineWidget : public QWidget {
  Q_OBJECT

 public:
  OutlineWidget(int width, int height, QWidget *parent = nullptr);
  ~OutlineWidget() override;

  void paint(QPainter *painter);

 private:
  QPixmap outline_;
};

}  // namespace suanzi

#endif
