#ifndef RECOGNIZE_TASK_H
#define RECOGNIZE_TASK_H

#include <QObject>

#include <chrono>

#include "config.hpp"
#include "detection_float.h"
#include "memory_pool.hpp"
#include "person_service.hpp"
#include "pingpang_buffer.h"
#include "quface_common.hpp"
#include "recognize_data.hpp"

namespace suanzi {
class RecognizeTask : QObject {
  Q_OBJECT
 public:
  RecognizeTask(FaceDatabasePtr db, FaceExtractorPtr extractor,
                PersonService::ptr person_service,
                MemoryPool<ImageBuffer, sizeof(ImageBuffer) * 5> *mem_pool,
                Config::ptr config, QThread *thread = nullptr,
                QObject *parent = nullptr);
  ~RecognizeTask();

 private slots:
  void rx_frame(PingPangBuffer<RecognizeData> *buffer);
  void rx_no_frame();

 signals:
  void tx_display(PersonData person);
  void tx_finish();
  void tx_record(int face_id, ImageBuffer *image_buffer);

 private:
  suanzi::FaceDetection to_detection(DetectionFloat detection_ratio, int width,
                                     int height);

  void query_success(const suanzi::QueryResult &person_info,
                     RecognizeData *img);
  void query_empty_database(RecognizeData *img);
  void query_no_face();

  bool sequence_query(std::vector<suanzi::QueryResult> history,
                      SZ_UINT32 &face_id);

  bool if_duplicated(SZ_UINT32 face_id);
  void report(SZ_UINT32 face_id, RecognizeData *img);

  FaceExtractorPtr face_extractor_;
  FaceDatabasePtr face_database_;
  PersonService::ptr person_service_;
  Config::ptr config_;

  std::vector<suanzi::QueryResult> history_;
  std::map<SZ_UINT32, std::chrono::steady_clock::time_point> query_clock_;

  MemoryPool<ImageBuffer, sizeof(ImageBuffer) * 5> *mem_pool_;
};

}  // namespace suanzi

#endif
