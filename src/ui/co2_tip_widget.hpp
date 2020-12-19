#ifndef _CO2_TIP_WIDGET_H
#define _CO2_TIP_WIDGET_H

#include <QLabel>
#include <QTimer>

namespace suanzi {

class Co2TipWidget : public QWidget {
  Q_OBJECT

 public:
  Co2TipWidget(int width, int height, QWidget *parent = nullptr);
  ~Co2TipWidget() override;

 private:
  void paintEvent(QPaintEvent *event) override;

 private slots:
  void rx_co2(int value);
  void delay_init_widgets();

 private:
  QLabel *pl_co2_;
  int cur_level_;
  int cur_ppm_value_;
};

}  // namespace suanzi

#endif
