#ifndef TOUCH_WIDGET_H
#define TOUCH_WIDGET_H

#include <QWidget>
#include <QStackedWidget>
#include "menu_key_widget.hpp"
#include "qrcode_widget.hpp"
#include "digit_key_widget.hpp"


namespace suanzi {

class TouchWidget : public QWidget {
  Q_OBJECT

 public:
  TouchWidget(int width, int height, QWidget *parent = nullptr);
  ~TouchWidget() override;

 private slots:
	void switch_stacked_widget(int index);

 private:
  	QStackedWidget *stacked_widget_;
	int screen_width_;
	int screen_height_;
};

}  // namespace suanzi

#endif
