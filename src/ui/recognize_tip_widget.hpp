#ifndef RECOGNIZETIPWIDGET_H
#define RECOGNIZETIPWIDGET_H

#include <QFont>
#include <QLabel>
#include <QPainterPath>
#include <QPixmap>
#include <QTimer>
#include <QWidget>

#include "person_service.hpp"

namespace suanzi {

class RecognizeTipWidget : public QWidget {
  Q_OBJECT

 public:
  RecognizeTipWidget(int width, int height, QWidget *parent = nullptr);
  ~RecognizeTipWidget() override;

 private:
  void check_temperature(bool &btemperature, bool &bnormal_temperature);

 signals:
  void tx_temperature(bool bvisible, bool bnormal_temperature,
                      float temperature);

 private slots:
  void rx_display(PersonData person, bool audio_duplicated,
                  bool record_duplicated);
  void rx_update();
  void rx_reset();
  void rx_timeout();

 private:
  PersonData person_;
  QPixmap snapshot_;
  QPixmap avatar_;

  QFont font_;
  QPainterPath temperature_rect_;
  QTimer reset_timer_;
  QTimer ip_timer_;

  bool has_info_;

  std::string ip_, name_, mac_;
  std::string hostname_;
  std::string serial_number_;
  std::string fw_version_;

  int screen_width_;
  int screen_height_;
  QLabel *pl_yyyy_mm_dd_;
  QLabel *pl_hh_mm_;
  QLabel *pl_week_;
  QLabel *pl_host_name_;
  QLabel *pl_sn_;
  QLabel *pl_avatar_;
  QLabel *pl_snapshot_;

  QString style_;
};

}  // namespace suanzi

#endif
