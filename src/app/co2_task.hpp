#ifndef __CO2_TASK_HPP__
#define __CO2_TASK_HPP__

#include <QThread>
#include <QTimer>
#include <quface-io/co2_reader.hpp>
#include <quface-io/option.hpp>

namespace suanzi {
using namespace io;

class Co2Task : public QThread {
  Q_OBJECT

 public:
  static Co2Task *get_instance();
  bool is_exist();

 private:
  bool check_co2();
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
  Co2Reader::ptr co2_reader_;
  bool has_co2_;
};

}  // namespace suanzi

#endif
