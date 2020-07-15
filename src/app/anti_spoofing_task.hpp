#ifndef ALIVE_TASK_H
#define ALIVE_TASK_H

#include <QObject>

#include "detection_float.h"
#include "image_package.h"
#include "pingpang_buffer.h"
#include "quface_common.hpp"
#include "recognize_data.hpp"
#include "config.hpp"

namespace suanzi {

class AntiSpoofingTask : QObject {
  Q_OBJECT
 public:
  AntiSpoofingTask(FaceAntiSpoofingPtr anti_spoofing, QThread *thread = nullptr,
                QObject *parent = nullptr);
  ~AntiSpoofingTask();

  bool is_person_alive();

 private slots:
  void rx_frame(PingPangBuffer<RecognizeData>* buffer);
  void rx_no_frame();
  void rx_finish();

 signals:
  void tx_finish();
  void tx_frame(PingPangBuffer<RecognizeData>* buffer);
  void tx_no_frame();

 private:
  FaceAntiSpoofingPtr anti_spoofing_;
  Config::ptr config_;

  std::vector<SZ_BOOL> history_;

  bool b_tx_ok_;
  bool b_data_ready_;
};

}  // namespace suanzi

#endif
