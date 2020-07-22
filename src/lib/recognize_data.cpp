#include "recognize_data.hpp"

using namespace suanzi;

RecognizeData::RecognizeData() {
  is_live = false;
  person_info.score = 0;
  person_info.face_id = 0;
}

RecognizeData::RecognizeData(Size size_bgr_large, Size size_bgr_small,
                             Size size_nir_large, Size size_nir_small)
    : DetectionData(size_bgr_large, size_bgr_small, size_nir_large,
                    size_nir_small) {
  is_live = false;
  person_info.score = 0;
  person_info.face_id = 0;
}

RecognizeData::~RecognizeData() {}