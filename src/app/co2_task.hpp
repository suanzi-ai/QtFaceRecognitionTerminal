#ifndef __CO2_TASK_HPP__
#define __CO2_TASK_HPP__

#include <QThread>
#include <QTimer>
#include <quface-io/option.hpp>

namespace suanzi {
using namespace io;

class Co2Task : public QThread {
  Q_OBJECT

 public:
  static Co2Task *get_instance();

 private:
  void run() override;

 signals:
  void tx_co2(int value);

 private slots:
  void read_co2();

 private:
  Co2Task(QObject *parent = nullptr);
  ~Co2Task();

 private:
  QTimer *reader_timer_;
};

}  // namespace suanzi

#endif
