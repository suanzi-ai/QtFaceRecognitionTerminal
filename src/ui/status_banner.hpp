#ifndef STATUS_BANNER_H
#define STATUS_BANNER_H

#include <QTimer>
#include <QWidget>
#include <QFont>
#include <QPixmap>

#include "quface_common.hpp"
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
  SZ_UINT32 db_size_;

  PersonService::ptr person_service_;
  FaceDatabasePtr db_;

  QTimer *timer_;
  QPixmap icon1_;
  QPixmap icon2_;
  QPixmap icon3_;
  QPixmap icon4_;
  QPixmap icon5_;
};

}  // namespace suanzi

#endif
