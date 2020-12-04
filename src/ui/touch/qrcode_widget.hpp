#ifndef QRCODE_KEY_WIDGET_H
#define QRCODE_KEY_WIDGET_H

#include <QWidget>

namespace suanzi {

class QrcodeWidget : public QWidget {
  Q_OBJECT

 public:
  QrcodeWidget(int width, int height, QWidget *parent = nullptr);
  ~QrcodeWidget() override;

 private:
 	void paintEvent(QPaintEvent *event) override;

 signals:
 	void switch_stacked_widget(int index);

 private slots:
 	void clicked_back();

 private:

};

}  // namespace suanzi

#endif
