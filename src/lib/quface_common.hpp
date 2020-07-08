#pragma once

#include <memory>
#include <quface/common.hpp>
#include <quface/db.hpp>
#include <quface/face.hpp>

typedef std::shared_ptr<suanzi::FaceDetector> FaceDetectorPtr;
typedef std::shared_ptr<suanzi::FaceExtractor> FaceExtractorPtr;
typedef std::shared_ptr<suanzi::FaceDatabase> FaceDatabasePtr;
