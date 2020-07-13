#ifndef RECOGNIZE_TASK_H
#define RECOGNIZE_TASK_H

#include <QObject>

#include "config.hpp"
#include "detection_float.h"
#include "recognize_data.hpp"
#include "person.hpp"
#include "person_service.hpp"
#include "pingpang_buffer.h"
#include "quface_common.hpp"
#include "memory_pool.hpp"

namespace suanzi {

class RecognizeTask : QObject {
  Q_OBJECT
 public:
  RecognizeTask(FaceDatabasePtr db, FaceExtractorPtr extractor,
                PersonService::ptr person_service, Config::ptr config,
                QThread *thread = nullptr, QObject *parent = nullptr);
  ~RecognizeTask();

 private slots:
  void rx_frame(PingPangBuffer<RecognizeData> *buffer);
  void rx_no_frame();

 signals:
  void tx_display(PersonDisplay person);
  void tx_finish();
  void tx_record(int face_id, ImageBuffer *image_buffer);

 private:
  suanzi::FaceDetection to_detection(DetectionFloat detection_ratio, int width,
                                     int height);

  //void report(SZ_UINT32 face_id, ImagePackage *img);
  void query_success(const suanzi::QueryResult &person_info, RecognizeData *img);
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

  MemoryPool<ImageBuffer, sizeof(ImageBuffer) * 5> mem_pool_;
};

}  // namespace suanzi

#endif
