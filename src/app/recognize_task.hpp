#ifndef RECOGNIZE_TASK_H
#define RECOGNIZE_TASK_H

#include <QObject>

#include "config.hpp"
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
                PersonService::ptr person_service, Config::ptr config,
                QThread *thread = nullptr, QObject *parent = nullptr);
  ~RecognizeTask();

 private slots:
  void rx_frame(PingPangBuffer<ImagePackage> *buffer, DetectionFloat detection);

 signals:
  void tx_display(PersonDisplay person);
  void tx_finish();

 private:
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
  PersonService::ptr person_service_;
  Config::ptr config_;

  std::vector<suanzi::QueryResult> history_;
  SZ_UINT32 last_face_id_ = 0;
};

}  // namespace suanzi

#endif
