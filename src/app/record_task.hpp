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
  static bool card_readed();

  static void clear_temperature();

 private slots:
  void rx_frame(PingPangBuffer<RecognizeData> *buffer);
  void rx_temperature(float body_temperature);

  void rx_card_readed(QString card_no);

  void rx_reset();

  void rx_enable(bool enable);

 signals:
  void tx_nir_finish(bool if_finished);
  void tx_bgr_finish(bool if_finished);

  // for display
  void tx_display(PersonData person, bool audio_duplicated,
                  bool record_duplicated);

 private:
  RecordTask(QThread *thread = nullptr, QObject *parent = nullptr);
  ~RecordTask();

  bool if_fresh(const FaceFeature &feature);
  void reset_recognize();
  void reset_temperature();

  bool sequence_query(const std::vector<QueryResult> &person_history,
                      const std::vector<bool> &mask_history,
                      const bool has_mask, SZ_UINT32 &face_id, SZ_FLOAT &score);
  bool sequence_antispoof(const std::vector<bool> &history, bool &is_live);
  bool sequence_mask(const std::vector<bool> &history, bool &has_mask);
  bool sequence_temperature(SZ_UINT32 face_id, int duration,
                            std::map<SZ_UINT32, float> &history,
                            float &temperature);
  bool update_temperature_bias();

  bool update_person_temperature(const SZ_UINT32 &face_id, int duration,
                                 PersonData &person);
  void update_person_info(RecognizeData *input, const SZ_UINT32 &face_id,
                          PersonData &person);
  void update_person_info(RecognizeData *input, const std::string &card_no,
                          PersonData &person);
  void update_person_snapshot(RecognizeData *input, PersonData &person);

  bool if_duplicated(SZ_UINT32 &face_id, const FaceFeature &feature,
                     int &duration, PersonData &person);
  bool if_temperature_updated(float &temperature);

  bool is_running_;

  PersonService::ptr person_service_;

  FaceDatabasePtr face_database_, unknown_database_;

  std::vector<QueryResult> person_history_;
  std::vector<bool> mask_history_;
  std::map<SZ_UINT32, float> known_temperature_;

  std::vector<bool> live_history_;
  std::map<SZ_UINT32, float> unknown_temperature_;

  std::chrono::steady_clock::time_point last_query_clock_;
  int duplicated_counter_;

  bool has_unhandle_person_;
  PersonData latest_person_;
  SZ_UINT32 duplicated_id_;
  int duplicated_duration_;

  FaceFeature latest_feature_;

  std::vector<float> temperature_history_;
  float latest_temperature_;

  bool has_card_no_;
  std::string card_no_;

  bool is_enabled_;
};

}  // namespace suanzi

#endif
