#ifndef QRCODE_TASK_H
#define QRCODE_TASK_H

#include <QThread>
#include <QTimer>
#include "person_service.hpp"

namespace suanzi {

class QrcodeTask : QThread {
  Q_OBJECT
 public:
  static QrcodeTask *get_instance();

 private:
  QrcodeTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~QrcodeTask();
  void run() override;
  bool scan_qrcode();

 private slots:
  void read_qrcode();
  void start_qrcode_task();
  void stop_qrcode_task();

 signals:
  void tx_qrcode(QString qrcode);
  void tx_detect_result(bool valid_detect);
  void tx_ok();

 private:
  PersonService::ptr person_service_;
  QTimer *reader_timer_;
};

}  // namespace suanzi

#endif
