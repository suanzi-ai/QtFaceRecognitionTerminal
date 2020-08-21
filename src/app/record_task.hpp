#ifndef RECORD_TASK_H
#define RECORD_TASK_H

#include <QObject>

#include <chrono>

#include "person_service.hpp"
#include "pingpang_buffer.hpp"
#include "quface_common.hpp"
#include "recognize_data.hpp"

namespace suanzi {

class RecordTask : QObject {
  Q_OBJECT
 public:
  RecordTask(PersonService::ptr person_service, QThread *thread = nullptr,
             QObject *parent = nullptr);

  ~RecordTask();

 private slots:
  void rx_frame(PingPangBuffer<RecognizeData> *buffer);
  void rx_reset();
  void rx_temperature(float body_temperature);

 signals:
  void tx_finish();

  void tx_nir_finish(bool if_finished);
  void tx_bgr_finish(bool if_finished);

  // for display
  void tx_display(PersonData person, bool if_duplicated);

 private:
  bool if_new(const FaceFeature &feature);

  bool sequence_query(const std::vector<QueryResult> &history,
                      SZ_UINT32 &face_id);
  bool sequence_antispoof(const std::vector<bool> &history);

  bool if_duplicated(const SZ_UINT32 &face_id);
  bool if_duplicated(const FaceFeature &feature);

  PersonService::ptr person_service_;

  FaceDatabasePtr unknown_database_;

  std::vector<QueryResult> person_history_;
  std::map<SZ_UINT32, std::chrono::steady_clock::time_point> query_clock_;

  std::vector<bool> live_history_;
  std::map<SZ_UINT32, std::chrono::steady_clock::time_point>
      unknown_query_clock_;

  int reset_counter_;
  float body_temperature_;

  FaceFeature last_feature_;
};

}  // namespace suanzi

#endif
