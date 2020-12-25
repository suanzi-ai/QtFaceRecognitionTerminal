#ifndef _TEMPERATURE_TIP_WIDGET_H
#define _TEMPERATURE_TIP_WIDGET_H

#include <QLabel>
#include <QTimer>

namespace suanzi {

class TemperatureTipWidget : public QWidget {
  Q_OBJECT

 public:
  TemperatureTipWidget(int width, int height, int parent_widget_pos_y,
                       QWidget *parent = nullptr);
  ~TemperatureTipWidget() override;

 public slots:
  void rx_temperature(bool bvisible, bool bnormal_temperature,
                      float temperature);

 private:
  QLabel *pl_ok_or_no_;
  QLabel *pl_temperature_;

  int radius_;
};

}  // namespace suanzi

#endif
