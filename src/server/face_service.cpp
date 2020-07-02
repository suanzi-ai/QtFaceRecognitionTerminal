#include "face_service.hpp"

#include <stdlib.h>

#include <opencv2/opencv.hpp>

#include "curl_util.hpp"
#include "logger.hpp"
#include "base64.hpp"

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

std::string FaceService::get_image_file_name(SZ_UINT32 faceId) {
  return std::to_string(faceId) + ".jpg";
}

bool FaceService::save_image(SZ_UINT32 faceId,
                             const std::vector<SZ_BYTE> &buffer) {
  std::string fName = get_image_file_name(faceId);
  std::ofstream fd(fName, std::ios::binary);
  if (fd.is_open()) {
    fd.write((char *)buffer.data(), buffer.size());
    return true;
  }
  return false;
}

bool FaceService::load_image(SZ_UINT32 faceId, std::vector<SZ_BYTE> &buffer) {
  std::string fName = get_image_file_name(faceId);
  std::ifstream fd(fName, std::ios::binary);
  if (fd.is_open()) {
    buffer = std::vector<SZ_BYTE>(std::istreambuf_iterator<char>(fd), {});
    return true;
  }
  return false;
}

SZ_RETCODE FaceService::extract_image_feature(
    SZ_UINT32 faceId, const std::vector<SZ_BYTE> &imgBuf,
    FaceFeature &feature) {
  cv::Mat rawData(1, imgBuf.size(), CV_8UC1, (void *)imgBuf.data());
  cv::Mat decodedImage = cv::imdecode(rawData, cv::IMREAD_COLOR);
  if (decodedImage.empty()) {
    SZ_LOG_ERROR("Decode image data failed!");
    return SZ_RETCODE_FAILED;
  }

  int width = decodedImage.cols;
  int height = decodedImage.rows;

  SZ_RETCODE ret;
  std::vector<FaceDetection> detections;
  ret = detector_->detect(decodedImage.ptr(), width, height, detections);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("face detect failed!");
    return ret;
  }
  if (detections.size() > 1) {
    SZ_LOG_ERROR("more than one faces!");
    ret = SZ_RETCODE_FAILED;
    return SZ_RETCODE_FAILED;
  }

  ret = extractor_->extract(decodedImage.ptr(), width, height, detections[0],
                            feature);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("extractor.extractFeature failed!");
    return ret;
  }

  if (!save_image(faceId, imgBuf)) {
    SZ_LOG_ERROR("Save image data failed!");
    return SZ_RETCODE_FAILED;
  }

  return SZ_RETCODE_OK;
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
                                    std::vector<SZ_BYTE> &imgBuf) {
  SZ_RETCODE ret;
  if (face.face_image.size() > 0) {
    auto buf = base64_decode(face.face_image.c_str());
    imgBuf = std::vector<SZ_BYTE>(buf.begin(), buf.end());
  } else if (face.face_url.size() > 0) {
    ret = suanzi::download_to_buffer(face.face_url, imgBuf);
    if (ret != SZ_RETCODE_OK) {
      return ret;
    }
  } else if (face.face_path.size() > 0) {
    std::ifstream fd(face.face_path);
    if (!fd.is_open()) {
      return SZ_RETCODE_FAILED;
    }
    imgBuf = std::vector<SZ_BYTE>(std::istreambuf_iterator<char>(fd),
                                  std::istreambuf_iterator<char>{});
  } else {
    return SZ_RETCODE_FAILED;
  }
}

json FaceService::db_add(const json &body) {
  try {
    auto face = body["person"].get<PersonImageInfo>();
    SZ_LOG_DEBUG("db.add id: {}", face.id);

    SZ_RETCODE ret;
    FaceFeature feature;
    SZ_INT32 featureSize;
    std::vector<SZ_BYTE> imgBuf;

    ret = read_buffer(face, imgBuf);
    if (ret != SZ_RETCODE_OK) {
      return {
          {"ok", false},
          {"message", "read image failed"},
          {"code", "READ_IMAGE_FAILED"},
      };
    }

    ret = extract_image_feature(face.id, imgBuf, feature);
    if (ret != SZ_RETCODE_OK) {
      return {
          {"ok", false},
          {"message", "extract face failed, no face? more than one?"},
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
    SZ_INT32 featureSize;

    json failedPersons;

    std::vector<std::string> urls;
    std::map<std::string, std::vector<SZ_BYTE>> imgBufs;
    for (auto &face : faceArrary) {
      urls.push_back(face.face_url);
    }
    ret = suanzi::download_to_buffer(urls, imgBufs);
    if (ret != SZ_RETCODE_OK) {
      SZ_LOG_ERROR("download_to_buffer failed!");
      return {
          {"ok", false},
          {"message", "failed"},
          {"code", "DOWNLOAD_FACE_FAILED"},
      };
    }

    for (auto &face : faceArrary) {
      SZ_LOG_DEBUG("[Add many] id = {}", face.id);

      auto pImgBuf = imgBufs.find(face.face_url);
      if (pImgBuf == imgBufs.end()) {
        failedPersons.push_back(
            json({{"id", face.id}, {"reason", "EXTRACT_FACE_FAILED"}}));
        SZ_LOG_WARN("[Add many] failed face id: {} reason: {}", face.id,
                    "DOWNLOAD_FACE_FAILED");
        continue;
      }
      auto imgBuf = pImgBuf->second;

      ret = extract_image_feature(face.id, imgBuf, feature);
      if (ret != SZ_RETCODE_OK) {
        failedPersons.push_back(
            json({{"id", face.id}, {"reason", "EXTRACT_FACE_FAILED"}}));
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
  auto faceID = body["id"].get<int>();
  SZ_RETCODE ret = db_->remove(faceID);
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

  if (body.contains("page")) {
    page = body["page"].get<int>();
  }
  if (body.contains("limit")) {
    limit = body["limit"].get<int>();
  }
  if (body.contains("with_image")) {
    with_image = body["with_image"].get<bool>();
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
  for (int i = (page - 1) * limit; i < personIDList.size() && i < page * limit;
       i++) {
    std::string faceBase64;
    if (with_image) {
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
