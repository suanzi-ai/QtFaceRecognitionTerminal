#ifndef RECOGNIZE_TASK_H
#define RECOGNIZE_TASK_H

#include <QObject>

#include "detection_float.h"
#include "image_package.h"
#include "person.h"
#include "pingpang_buffer.h"

#include "quface/db.hpp"
#include "quface/face.hpp"

namespace suanzi {

class RecognzieTask : QObject {
  Q_OBJECT
 public:
  RecognzieTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~RecognzieTask();

 private slots:
  void rx_frame(PingPangBuffer<ImagePackage> *buffer, DetectionFloat detection);

 signals:
  void tx_result(Person person);
  void tx_finish();

 private:
  static constexpr int HISTORY_SIZE = 10;

  static constexpr int MIN_RECOGNIZE_COUNT = 6;
  static constexpr float MIN_RECOGNIZE_SCORE = 0.65f;
  static constexpr float MIN_ACCUMULATE_SCORE = 3.6f; // 0.6 * 6;

  static constexpr int MAX_LOST_AGE = 10;

  suanzi::FaceDetection to_detection(DetectionFloat detection_ratio, int width,
                                     int height);

  bool query(std::vector<suanzi::QueryResult> history, int &face_id);

  suanzi::FaceExtractor *face_extractor_;
  suanzi::FaceDatabase *face_database_;

  std::vector<suanzi::QueryResult> history_;

  int lost_age_;
};

}  // namespace suanzi

#endif
