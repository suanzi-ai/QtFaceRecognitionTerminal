#ifndef RECOGNIZE_TASK_H
#define RECOGNIZE_TASK_H

#include <QObject>

#include "detection_float.h"
#include "image_package.h"
#include "person.hpp"
#include "person_service.hpp"
#include "pingpang_buffer.h"
#include "quface_common.hpp"

namespace suanzi {

class RecognizeTask : QObject {
  Q_OBJECT
 public:
  RecognizeTask(FaceDatabasePtr db, FaceExtractorPtr extractor,
                PersonService::ptr person_service, QThread *thread = nullptr,
                QObject *parent = nullptr);
  ~RecognizeTask();

 private slots:
  void rx_frame(PingPangBuffer<ImagePackage> *buffer, DetectionFloat detection);

 signals:
  void tx_display(PersonDisplay person);
  void tx_finish();

 private:
  static constexpr int HISTORY_SIZE = 15;

  static constexpr int MIN_RECOGNIZE_COUNT = 10;
  static constexpr float MIN_RECOGNIZE_SCORE = 0.75f;
  static constexpr float MIN_ACCUMULATE_SCORE = 7.0f;  // 0.7 * 10;

  static constexpr int MAX_LOST_AGE = 20;

  suanzi::FaceDetection to_detection(DetectionFloat detection_ratio, int width,
                                     int height);

  void report(SZ_UINT32 face_id, ImagePackage *img);
  void query_success(const suanzi::QueryResult &person_info, ImagePackage *img);
  void query_empty_database();
  void query_no_face();

  bool sequence_query(std::vector<suanzi::QueryResult> history,
                      SZ_UINT32 &face_id);

  FaceExtractorPtr face_extractor_;
  FaceDatabasePtr face_database_;
  suanzi::PersonService::ptr person_service_;

  std::vector<suanzi::QueryResult> history_;
  SZ_UINT32 last_face_id_ = 0;
};

}  // namespace suanzi

#endif
