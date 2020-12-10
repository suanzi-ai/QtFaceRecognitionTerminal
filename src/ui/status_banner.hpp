#ifndef STATUS_BANNER_H
#define STATUS_BANNER_H

#include <QFont>
#include <QLabel>
#include <QPixmap>
#include <QTimer>
#include <QWidget>

#include "person_service.hpp"
#include "quface_common.hpp"

namespace suanzi {

class StatusBanner : public QWidget {
  Q_OBJECT

 public:
  StatusBanner(int width, int height, QWidget *parent = nullptr);
  ~StatusBanner() override;

 private slots:
  void rx_update();
  void rx_display(bool invisible);

  void rx_temperature(bool bvisible, bool bnormal_temperature,
                      float temperature);

 private:
  SZ_UINT32 db_size_;

  PersonService::ptr person_service_;
  FaceDatabasePtr db_;

  QTimer *timer_;
  QLabel *pl_person_num_;
  QLabel *pl_temperature_;
  QLabel *pl_net_;

  std::string name_, ip_, mac_, last_name_;

  QString style_;
};

}  // namespace suanzi

#endif
