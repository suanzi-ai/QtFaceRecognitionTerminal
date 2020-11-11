#include "face_service.hpp"

#include <algorithm>
#include <cstdlib>

#include <opencv2/opencv.hpp>

#include <quface/logger.hpp>

#include "base64.hpp"
#include "config.hpp"

#define MAX_PERSON_INFO_SIZE 1024

void suanzi::to_json(json &j, const PersonImageInfo &p) {
  j = json{
      {"id", p.id},
      {"faceUrl", p.face_url},
      {"facePath", p.face_path},
      {"faceImage", p.face_image},
  };
}

void suanzi::from_json(const json &j, PersonImageInfo &p) {
  j.at("id").get_to(p.id);

  if (j.contains("faceUrl")) {
    j.at("faceUrl").get_to(p.face_url);
  }

  if (j.contains("facePath")) {
    j.at("facePath").get_to(p.face_path);
  }

  if (j.contains("faceImage")) {
    j.at("faceImage").get_to(p.face_image);
  }
}

using namespace suanzi;

FaceService::FaceService(PersonService::ptr person_service, bool store_image)
    : person_service_(person_service),
      image_store_dir_(person_service->image_store_path_),
      store_image_(store_image) {
  auto quface = Config::get_quface();
  db_ = std::make_shared<FaceDatabase>(quface.db_name);

  detector_ = std::make_shared<FaceDetector>(quface.model_file_path);
  extractor_ = std::make_shared<FaceExtractor>(quface.model_file_path);
  pose_estimator_ = std::make_shared<FacePoseEstimator>(quface.model_file_path);
}
FaceService::~FaceService() {}

std::string FaceService::get_image_file_name(SZ_UINT32 face_id) {
  return std::to_string(face_id) + ".jpg";
}

bool FaceService::save_image(SZ_UINT32 face_id,
                             const std::vector<SZ_BYTE> &buffer) {
  if (!store_image_) {
    return true;
  }

  std::string fName = get_image_file_name(face_id);
  std::ofstream fd(fName, std::ios::binary);
  if (fd.is_open()) {
    fd.write((char *)buffer.data(), buffer.size());
    return true;
  }
  return false;
}

bool FaceService::load_image(SZ_UINT32 face_id, std::vector<SZ_BYTE> &buffer) {
  if (!store_image_) {
    return true;
  }

  std::string fName = get_image_file_name(face_id);
  std::ifstream fd(fName, std::ios::binary);
  if (fd.is_open()) {
    buffer = std::vector<SZ_BYTE>(std::istreambuf_iterator<char>(fd), {});
    return true;
  }
  return false;
}

SZ_RETCODE FaceService::extract_image_feature(SZ_UINT32 face_id,
                                              std::vector<SZ_BYTE> &buffer,
                                              FaceFeature &feature,
                                              std::string &error_message) {
  SZ_LOG_INFO("start extract");
  cv::Mat raw_data(1, buffer.size(), CV_8UC1, (void *)buffer.data());
  cv::Mat decoded_image = cv::imdecode(raw_data, cv::IMREAD_COLOR);
  SZ_LOG_INFO("end extract");
  if (decoded_image.empty()) {
    error_message = "cv::imdecode failed!";
    SZ_LOG_ERROR(error_message);
    return SZ_RETCODE_FAILED;
  }

  int width = decoded_image.cols;
  int height = decoded_image.rows;

  SZ_RETCODE ret;
  std::vector<FaceDetection> detections;
  FacePose pose;

  size_t size = width * height * 3;
  SZ_BYTE *bgr = new SZ_BYTE[size];
  std::copy(decoded_image.ptr(), decoded_image.ptr() + size, bgr);

  do {
    ret = detector_->detect(bgr, width, height, detections);
    if (ret != SZ_RETCODE_OK) {
      error_message = "assert detector_->detect == SZ_RETCODE_OK failed";
      SZ_LOG_ERROR(error_message);
      break;
    }

    if (detections.size() == 0) {
      error_message = "no face detected";
      SZ_LOG_ERROR(error_message);
      ret = SZ_RETCODE_FAILED;
      break;
    }

    if (detections.size() > 1) {
      error_message = "more than one face";
      SZ_LOG_ERROR(error_message);
      ret = SZ_RETCODE_FAILED;
      break;
    }

    ret = pose_estimator_->estimate(bgr, width, height, detections[0], pose);
    if (ret != SZ_RETCODE_OK) {
      error_message =
          "assert pose_estimator_->estimate == SZ_RETCODE_OK failed";
      SZ_LOG_ERROR(error_message);
      break;
    }
    auto cfg = Config::get_detect();
    if (!(cfg.min_yaw <= pose.yaw && pose.yaw <= cfg.max_yaw &&
          cfg.min_pitch <= pose.pitch && pose.pitch <= cfg.max_pitch &&
          cfg.min_roll <= pose.roll && pose.roll <= cfg.max_roll)) {
      error_message = "large pose face";
      SZ_LOG_ERROR(error_message);
      break;
    }

    ret = extractor_->extract(bgr, width, height, detections[0], pose, feature);
    if (ret != SZ_RETCODE_OK) {
      error_message = "assert extractor_->extract == SZ_RETCODE_OK failed";
      SZ_LOG_ERROR(error_message);
      break;
    }

    // save cropped and resized avatar
    static cv::Mat avatar;
    float center_x = detections[0].bbox.x + detections[0].bbox.width / 2;
    float center_y = detections[0].bbox.y + detections[0].bbox.height / 2;

    int avatar_size =
        std::min({detections[0].bbox.width, detections[0].bbox.height, center_x,
                  center_y, (float)width - center_x, (float)height - center_y});
    int avatar_x = center_x - avatar_size;
    int avatar_y = center_y - avatar_size;
    int avatar_w = avatar_size * 2;
    int avatar_h = avatar_size * 2;

    if (avatar_w > avatar_h)
      cv::resize(decoded_image({avatar_x, avatar_y, avatar_w, avatar_h}),
                 avatar, {200, (int)(200.f * avatar_h / avatar_w)});
    else
      cv::resize(decoded_image({avatar_x, avatar_y, avatar_w, avatar_h}),
                 avatar, {(int)(200.f * avatar_w / avatar_h), 200});

    buffer.clear();
    cv::imencode(".jpg", avatar, buffer);

    if (!save_image(face_id, buffer)) {
      SZ_LOG_ERROR("Save image data failed!");
      ret = SZ_RETCODE_FAILED;
      break;
    }
  } while (0);

  delete bgr;
  SZ_LOG_INFO("Success!");

  return ret;
}

SZ_RETCODE FaceService::read_image_as_base64(SZ_UINT32 id,
                                             std::string &result) {
  std::vector<SZ_BYTE> buffer;
  if (load_image(id, buffer)) {
    result = base64_encode(buffer.data(), buffer.size());
    return SZ_RETCODE_OK;
  }
  return SZ_RETCODE_FAILED;
}

SZ_RETCODE FaceService::read_buffer(const PersonImageInfo &face,
                                    std::vector<SZ_BYTE> &buffer) {
  SZ_RETCODE ret;
  if (face.face_path.size() > 0) {
    std::ifstream fd(image_store_dir_ + face.face_path);
    if (!fd.is_open()) {
      SZ_LOG_DEBUG("Read file {} failed", face.face_path);
      return SZ_RETCODE_FAILED;
    }
    buffer = std::vector<SZ_BYTE>(std::istreambuf_iterator<char>(fd),
                                  std::istreambuf_iterator<char>{});
  } else if (face.face_image.size() > 0) {
    auto buf = base64_decode(face.face_image.c_str());
    buffer = std::vector<SZ_BYTE>(buf.begin(), buf.end());
  } else {
    SZ_LOG_ERROR("No image found");
    return SZ_RETCODE_FAILED;
  }
  return SZ_RETCODE_OK;
}

json FaceService::db_add(const json &body) {
  try {
    SZ_LOG_INFO("start db_add");
    SZ_UINT32 db_size;
    db_->size(db_size);
    if (db_size >= MAX_DATABASE_SIZE) {
      return {
          {"ok", false},
          {"message", "max database size(2.5w) exceeds"},
          {"code", "MAX_DATABASE_SIZE_EXCEEDS"},
      };
    }

    auto face = body["person"].get<PersonImageInfo>();
    SZ_LOG_DEBUG("db.add id: {}", face.id);

    SZ_RETCODE ret;
    FaceFeature feature;
    SZ_INT32 feature_size;
    std::vector<SZ_BYTE> buffer;

    ret = read_buffer(face, buffer);
    if (ret != SZ_RETCODE_OK) {
      return {
          {"ok", false},
          {"message", "read image failed"},
          {"code", "READ_IMAGE_FAILED"},
      };
    }

    std::string error_message;
    ret = extract_image_feature(face.id, buffer, feature, error_message);
    if (ret != SZ_RETCODE_OK) {
      SZ_LOG_ERROR("extract_image_feature failed");
      return {
          {"ok", false},
          {"message", error_message},
          {"code", "EXTRACT_FACE_FAILED"},
      };
    }

    ret = db_->add(face.id, feature);
    if (ret != SZ_RETCODE_OK) {
      SZ_LOG_ERROR("db.add failed");
      return {
          {"ok", false},
          {"message", "db add failed"},
          {"code", "DB_FAILED"},
      };
    }

    ret = person_service_->update_person_face_image(face.id, buffer);
    if (ret != SZ_RETCODE_OK) {
      SZ_LOG_ERROR("update_person_face_image failed");
      return {
          {"ok", false},
          {"message", "update_person_face_image failed"},
          {"code", "DB_FAILED"},
      };
    }

    ret = db_->save();
    if (ret != SZ_RETCODE_OK) {
      SZ_LOG_ERROR("db.save failed");
      return {
          {"ok", false},
          {"message", "db save failed"},
          {"code", "UPDATE_AVATAR"},
      };
    }

    return {{"ok", true}, {"message", "ok"}};
  } catch (std::exception &e) {
    SZ_LOG_ERROR("received data error {}", e.what());
    return {
        {"ok", false},
        {"message", e.what()},
        {"code", "UNKNOWN"},
    };
  }
}

json FaceService::db_add_many(const json &body) {
  try {
    auto faceArrary = body["persons"].get<std::vector<PersonImageInfo>>();

    SZ_RETCODE ret;
    FaceFeature feature;
    SZ_INT32 feature_size;

    json failedPersons;

    std::vector<SZ_BYTE> buffer;

    for (auto &face : faceArrary) {
      SZ_UINT32 db_size;
      db_->size(db_size);
      if (db_size >= MAX_DATABASE_SIZE) {
        return {
            {"ok", false},
            {"message", "max database size(2.5w) exceeds"},
            {"code", "MAX_DATABASE_SIZE_EXCEEDS"},
        };
      }

      SZ_LOG_DEBUG("[Add many] id = {}", face.id);

      ret = read_buffer(face, buffer);
      if (ret != SZ_RETCODE_OK) {
        return {
            {"ok", false},
            {"message", "read image failed"},
            {"code", "READ_IMAGE_FAILED"},
        };
      }

      std::string error_message;
      ret = extract_image_feature(face.id, buffer, feature, error_message);
      if (ret != SZ_RETCODE_OK) {
        failedPersons.push_back(
            json({{"id", face.id},
                  {"reason", "EXTRACT_FACE_FAILED: " + error_message}}));
        SZ_LOG_WARN("[Add many] failed face id: {} reason: {}", face.id,
                    "EXTRACT_FACE_FAILED");
        continue;
      }

      ret = db_->add(face.id, feature);
      if (ret != SZ_RETCODE_OK) {
        failedPersons.push_back(
            json({{"id", face.id}, {"reason", "DB_FAILED"}}));
        SZ_LOG_WARN("[Add many] failed face id: {} reason: {}", face.id,
                    "DB_FAILED");
        continue;
      }

      ret = person_service_->update_person_face_image(face.id, buffer);
      if (ret != SZ_RETCODE_OK) {
        failedPersons.push_back(
            json({{"id", face.id}, {"reason", "UPDATE_AVATAR"}}));
        SZ_LOG_WARN("[Add many] failed face id: {} reason: {}", face.id,
                    "UPDATE_AVATAR");
        continue;
      }
    }

    ret = db_->save();
    if (ret != SZ_RETCODE_OK) {
      SZ_LOG_ERROR("[Add many] db.save failed");
      return {
          {"ok", false},
          {"message", "db save failed"},
          {"code", "DB_FAILED"},
      };
    }

    SZ_LOG_INFO("[Add many] success {} faces, failed {} faces",
                faceArrary.size() - failedPersons.size(), failedPersons.size());

    if (failedPersons.size() > 0) {
      return {
          {"ok", true},
          {"message", "some of the faces are failed"},
          {"failedPersons", failedPersons},
          {"code", "PARTIAL_FAILED"},
      };
    }

    return {{"ok", true}, {"message", "ok"}};
  } catch (std::exception &e) {
    SZ_LOG_ERROR("received data error {}", e.what());
    return {{"ok", false}, {"message", e.what()}};
  }
}

json FaceService::db_remove_by_id(const json &body) {
  SZ_LOG_DEBUG("db.remove_by_id");
  auto face_id = body["id"].get<int>();
  SZ_RETCODE ret = db_->remove(face_id);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("db_->remove failed!");
    return {
        {"ok", false},
        {"message", "failed"},
        {"code", "DB_FAILED"},
    };
  }

  ret = db_->save();
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("db.save failed");
    return {
        {"ok", false},
        {"message", "db save failed"},
        {"code", "DB_FAILED"},
    };
  }

  return {{"ok", true}, {"message", "ok"}};
}

json FaceService::db_remove_all(const json &body) {
  SZ_LOG_DEBUG("db.remove_all");
  SZ_RETCODE ret = db_->clear();
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("db.clear failed");
    return {
        {"ok", false},
        {"message", "db clear failed"},
        {"code", "DB_FAILED"},
    };
  }

  ret = db_->save();
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("db.save failed");
    return {
        {"ok", false},
        {"message", "db save failed"},
        {"code", "DB_FAILED"},
    };
  }
  return {{"ok", true}, {"message", "ok"}};
}

json FaceService::db_get_all(const json &body) {
  int page = 1;
  int limit = 10;
  bool with_image = false;
  bool no_pagination = false;

  if (body.contains("page")) {
    page = body["page"].get<int>();
  }
  if (body.contains("limit")) {
    limit = body["limit"].get<int>();
  }
  if (body.contains("with_image")) {
    with_image = body["with_image"].get<bool>();
  }
  if (body.contains("no_pagination")) {
    no_pagination = body["no_pagination"].get<bool>();
  }

  SZ_LOG_DEBUG("db.get_all");
  std::vector<SZ_UINT32> personIDList;
  SZ_RETCODE ret = db_->list(personIDList);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("db_->list failed!");
    return {
        {"ok", false},
        {"message", "failed"},
        {"code", "DB_FAILED"},
    };
  }

  std::vector<PersonImageInfo> persons;
  int start = (page - 1) * limit;
  int end = page * limit;
  if (no_pagination) {
    start = 0;
    end = personIDList.size();
  }

  for (int i = start; i < personIDList.size() && i < end; i++) {
    std::string faceBase64;
    if (with_image && store_image_) {
      SZ_RETCODE ret = read_image_as_base64(personIDList[i], faceBase64);
      if (ret != SZ_RETCODE_OK) {
        SZ_LOG_ERROR("Read image #{} as base64 failed", personIDList[i]);
      }
    }

    persons.push_back(PersonImageInfo{
        .id = personIDList[i],
        .face_url = "",
        .face_path = "",
        .face_image = faceBase64,
    });
  }

  return {{"ok", true}, {"persons", persons}, {"total", personIDList.size()}};
}
