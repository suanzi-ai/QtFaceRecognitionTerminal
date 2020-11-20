#ifndef OUTLINE_WIDGET_H
#define OUTLINE_WIDGET_H

#include <QTimer>
#include <QWidget>

namespace suanzi {

class OutlineWidget : public QWidget {
  Q_OBJECT

 public:
  OutlineWidget(int width, int height, QWidget *parent = nullptr);
  ~OutlineWidget() override;

  void paint(QPainter *painter);

 private slots:
  void rx_warn_distance();
  void rx_reset();

 private:
  QTimer timer_;

  bool show_valid_rect_;
};

}  // namespace suanzi

#endif
