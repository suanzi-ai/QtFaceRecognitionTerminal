#ifndef __READER_TASK_HPP__
#define __READER_TASK_HPP__

#include <QThread>
#include <quface-io/option.hpp>
#include <quface-io/ic_reader.hpp>

namespace suanzi {
using namespace io;

class ReaderTask : public QThread {
  Q_OBJECT

 public:
  ReaderTask();
  ~ReaderTask();

 protected:
 signals:
  

 private slots:
  void rx_enable();
  void rx_disable();

 private:
  void run();

 private:
  ICReader::ptr ic_reader_;
  bool is_enabled_;
};

}  // namespace suanzi

#endif
