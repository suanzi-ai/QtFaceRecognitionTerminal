#pragma once

#include <nlohmann/json.hpp>

#include "person_service.hpp"
#include "quface_common.hpp"

namespace suanzi {
using json = nlohmann::json;

struct PersonImageInfo {
  SZ_UINT32 id;
  std::string face_url;
  std::string face_path;
  std::string face_image;
};

void to_json(json &j, const PersonImageInfo &p);

void from_json(const json &j, PersonImageInfo &p);

class FaceService {
 public:
  typedef std::shared_ptr<FaceService> ptr;

  FaceService(FaceDatabasePtr db, FaceDetectorPtr detector,
              FacePoseEstimatorPtr pose_estimator, FaceExtractorPtr extractor,
              PersonService::ptr person_service,
              const std::string &image_store_dir, bool store_image = false)
      : db_(db),
        detector_(detector),
        pose_estimator_(pose_estimator),
        extractor_(extractor),
        person_service_(person_service),
        image_store_dir_(image_store_dir),
        store_image_(store_image) {}
  ~FaceService() {}

  json db_add(const json &body);
  json db_add_many(const json &body);
  json db_remove_by_id(const json &body);
  json db_remove_all(const json &body);
  json db_get_all(const json &body);

 private:
  static constexpr size_t MAX_DATABASE_SIZE = 25000;

  SZ_RETCODE read_buffer(const PersonImageInfo &face,
                         std::vector<SZ_BYTE> &imgBuf);
  std::string get_image_file_name(SZ_UINT32 faceId);
  bool save_image(SZ_UINT32 faceId, const std::vector<SZ_BYTE> &buffer);
  bool load_image(SZ_UINT32 faceId, std::vector<SZ_BYTE> &buffer);
  SZ_RETCODE extract_image_feature(SZ_UINT32 faceId,
                                   std::vector<SZ_BYTE> &imgBuf,
                                   FaceFeature &pFeature);
  SZ_RETCODE read_image_as_base64(SZ_UINT32 id, std::string &result);

  FaceDatabasePtr db_;
  FaceDetectorPtr detector_;
  FacePoseEstimatorPtr pose_estimator_;
  FaceExtractorPtr extractor_;
  PersonService::ptr person_service_;

  std::string image_store_dir_;
  bool store_image_;
};

}  // namespace suanzi
