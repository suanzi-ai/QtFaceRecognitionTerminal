#ifndef TOUCH_WIDGET_H
#define TOUCH_WIDGET_H

#include <QStackedWidget>

#include "digit_key_widget.hpp"
#include "menu_key_widget.hpp"
#include "qrcode_widget.hpp"

namespace suanzi {

class TouchWidget : public QStackedWidget {
  Q_OBJECT

 public:
  TouchWidget(int width, int height, QWidget *parent = nullptr);
  ~TouchWidget() override;

 signals:
  void tx_enable_face_recognition(bool enable);

 private slots:
  void switch_stacked_widget(int index);
  void rx_display(bool invisible);

 private:
  QStackedWidget *stacked_widget_;
  QrcodeWidget *qrcode_widget_;
  DigitKeyWidget *digit_key_widget_;
  MenuKeyWidget *menu_key_widget_;
  int screen_width_;
  int screen_height_;
  QWidget *parent_widget_;
};

}  // namespace suanzi

#endif
