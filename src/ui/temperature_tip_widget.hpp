#ifndef _TEMPERATURE_TIP_WIDGET_H
#define _TEMPERATURE_TIP_WIDGET_H

#include <QLabel>
#include <QTimer>

namespace suanzi {

class TemperatureTipWidget : public QWidget {
  Q_OBJECT

 public:
  TemperatureTipWidget(int width, int height, QWidget *parent = nullptr);
  ~TemperatureTipWidget() override;

 private slots:
  void rx_temperature(bool bvisible, bool bnormal_temperature,
                      float temperature);
  void rx_reset();

 private:
  QLabel *pl_ok_or_no_;
  QLabel *pl_temperature_;
  QTimer reset_timer_;

  int radius_;
};

}  // namespace suanzi

#endif
