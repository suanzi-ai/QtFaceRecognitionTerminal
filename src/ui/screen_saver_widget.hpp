#ifndef SCREEN_SAVER_WIDGET_H
#define SCREEN_SAVER_WIDGET_H

#include <QImage>
#include <QPixmap>
#include <QTimer>
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
  void rx_display(bool visible);
  void resfresh_timeout();

 private:
  QTimer *refresh_timer_;
};

}  // namespace suanzi

#endif
