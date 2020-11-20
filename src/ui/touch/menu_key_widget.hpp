#ifndef MENU_KEY_WIDGET_H
#define MENU_KEY_WIDGET_H

#include <QWidget>

namespace suanzi {

class MenuKeyWidget : public QWidget {
  Q_OBJECT

 public:
  MenuKeyWidget(int width, int height, QWidget *parent = nullptr);
  ~MenuKeyWidget() override;

 private:

 signals:
 	void tx_menu_type(int menu_type);
 	void switch_stacked_widget(int index);


 private slots:
	void clicked_qrcode();
	void clicked_intercom();
	void clicked_password();


 private:

};

}  // namespace suanzi

#endif
