#ifndef QRCODE_KEY_WIDGET_H
#define QRCODE_KEY_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>

namespace suanzi {

class QrcodeWidget : public QWidget {
  Q_OBJECT

 public:
  QrcodeWidget(int width, int height, QWidget *parent = nullptr);
  ~QrcodeWidget() override;
  void init();

 private:
 	void paintEvent(QPaintEvent *event) override;

 signals:
 	void switch_stacked_widget(int index);

 private slots:
 	void clicked_back();
	void scan_qrcode();

 private:
	QTimer *timer_;
	QLabel *pl_lightneedle_;
	int pos_x_;
	int pos_y_;
	const int QRCODE_WIDTH = 400;
	const int QRCODE_HEIGHT = 400;
};

}  // namespace suanzi

#endif
