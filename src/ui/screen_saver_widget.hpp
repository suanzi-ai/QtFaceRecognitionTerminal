#ifndef SCREEN_SAVER_WIDGET_H
#define SCREEN_SAVER_WIDGET_H

#include <QPixmap>
#include <QImage>
#include <QWidget>

namespace suanzi {

class ScreenSaverWidget : public QWidget {
  Q_OBJECT

 public:
  ScreenSaverWidget(int width, int height, QWidget *parent = nullptr);
  ~ScreenSaverWidget() override;

 protected:
  void paintEvent(QPaintEvent *event);

 private slots:
  void rx_display(int disappear_seconds);
  void rx_hide();

 private:
  QImage background_;
};

}  // namespace suanzi

#endif
