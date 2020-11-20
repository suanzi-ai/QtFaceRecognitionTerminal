#ifndef RECOGNIZE_DATA_H
#define RECOGNIZE_DATA_H

#include <QMetaType>

#include "detection_data.hpp"
#include "quface_common.hpp"

namespace suanzi {

class RecognizeData : public DetectionData {
 public:
  RecognizeData();
  RecognizeData(Size size_bgr_large, Size size_bgr_small, Size size_nir_large,
                Size size_nir_small);

  ~RecognizeData();

  bool has_live;
  bool is_live;

  bool has_person_info;
  QueryResult person_info;
  FaceFeature person_feature;
  bool has_mask;
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::RecognizeData);

#endif
