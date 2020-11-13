#ifndef RECORD_TASK_H
#define RECORD_TASK_H

#include <chrono>

#include <QObject>
#include <QTimer>

#include "person_service.hpp"
#include "pingpang_buffer.hpp"
#include "quface_common.hpp"
#include "recognize_data.hpp"

namespace suanzi {

class RecordTask : QObject {
  Q_OBJECT
 public:
  static RecordTask *get_instance();
  static bool idle();

 private slots:
  void rx_frame(PingPangBuffer<RecognizeData> *buffer);
  void rx_reset_recognizing();
  void rx_temperature(float body_temperature);
  void rx_start_temperature();
  void rx_reset_temperature();
  void rx_end_temperature();

 signals:
  void tx_nir_finish(bool if_finished);
  void tx_bgr_finish(bool if_finished);

  // for display
  void tx_display(PersonData person, bool if_duplicated);

  // for audio report
  void tx_report_person(PersonData person);
  void tx_report_temperature(PersonData person);
  void tx_warn_mask();

 private:
  RecordTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~RecordTask();

  bool if_fresh(const FaceFeature &feature);

  bool sequence_query(const std::vector<QueryResult> &history,
                      SZ_UINT32 &face_id, SZ_FLOAT &score);
  bool sequence_antispoof(const std::vector<bool> &history);
  bool sequence_mask_detecting(const std::vector<bool> &history);
  bool sequence_temperature(const SZ_UINT32 &face_id, int duration,
                            std::map<SZ_UINT32, float> &history,
                            float &temperature);

  bool if_duplicated(const SZ_UINT32 &face_id, float &temperature);
  bool if_duplicated(const FaceFeature &feature, float &temperature);

  bool is_running_;

  PersonService::ptr person_service_;

  FaceDatabasePtr db_, unknown_database_;

  std::vector<QueryResult> person_history_;
  std::vector<bool> mask_history_;
  std::map<SZ_UINT32, std::chrono::steady_clock::time_point> query_clock_;
  std::map<SZ_UINT32, float> known_temperature_;

  std::vector<bool> live_history_;
  std::map<SZ_UINT32, std::chrono::steady_clock::time_point>
      unknown_query_clock_;
  std::map<SZ_UINT32, float> unknown_temperature_;

  int reset_counter_;

  FaceFeature last_feature_;

  bool is_measuring_temperature_;
  std::vector<float> temperature_history_;
  float max_temperature_;
  QTimer *temperature_timer_;
};

}  // namespace suanzi

#endif
