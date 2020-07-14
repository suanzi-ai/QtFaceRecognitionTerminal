#ifndef DETECTION_FLOAT_H
#define DETECTION_FLOAT_H

#include <QMetaType>
#include <quface_common.hpp>

struct DetectionRatio {
  float x;
  float y;
  float width;
  float height;
  float landmark[SZ_LANDMARK_NUM][2];
  bool b_valid;
  int frame_idx;

  suanzi::FaceDetection to_detection(int width, int height) {
    suanzi::FaceDetection face_detection;
    face_detection.bbox.x = x * width;
    face_detection.bbox.y = y * height;
    face_detection.bbox.width = width * width;
    face_detection.bbox.height = height * height;

    for (int i = 0; i < SZ_LANDMARK_NUM; i++) {
      face_detection.landmarks.point[i].x =
          landmark[i][0] * width;
      face_detection.landmarks.point[i].y =
          landmark[i][1] * height;
    }

    return face_detection;
  }
};

Q_DECLARE_METATYPE(DetectionRatio);

#endif
