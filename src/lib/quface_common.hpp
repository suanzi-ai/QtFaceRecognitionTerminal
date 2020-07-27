#pragma once

#include <memory>
#include <quface/common.hpp>
#include <quface/db.hpp>
#include <quface/face.hpp>

namespace suanzi {
typedef std::shared_ptr<FaceDetector> FaceDetectorPtr;
typedef std::shared_ptr<FaceExtractor> FaceExtractorPtr;
typedef std::shared_ptr<FaceDatabase> FaceDatabasePtr;
typedef std::shared_ptr<FacePoseEstimator> FacePoseEstimatorPtr;
typedef std::shared_ptr<FaceAntiSpoofing> FaceAntiSpoofingPtr;
}  // namespace suanzi
