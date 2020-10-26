#ifndef STATUS_BANNER_H
#define STATUS_BANNER_H

#include <QTimer>
#include <QWidget>
#include <QFont>
#include <QPixmap>

#include "person_service.hpp"

namespace suanzi {

class StatusBanner : public QWidget {
  Q_OBJECT

 public:
  StatusBanner(int width, int height, QWidget *parent = nullptr);
  ~StatusBanner() override;

  void paint(QPainter *painter);

 private slots:
  void rx_update();

 private:
  std::string ip_address_;
  std::string version_;

  PersonService::ptr person_service_;
  QTimer *timer_;
  QFont font_;
  QPixmap icon_;
  QPixmap icon2_;
  QPixmap icon3_;
  QPixmap icon4_;
  QPixmap icon5_;
  QPixmap icon6_;
  QPixmap icon7_;
};

}  // namespace suanzi

#endif
