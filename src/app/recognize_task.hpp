#ifndef RECOGNIZE_TASK_H
#define RECOGNIZE_TASK_H

#include <QObject>

#include "detection_float.h"
#include "image_package.h"
#include "pingpang_buffer.h"
#include "person.h"

#include "quface/face.hpp"

namespace suanzi {

class RecognzieTask : QObject {
  Q_OBJECT
 public:
    RecognzieTask(QThread *pThread = nullptr, QObject *parent = nullptr);
    ~RecognzieTask();

 private slots:
    void rxFrame(PingPangBuffer<ImagePackage> *buffer, DetectionFloat detection);

 signals:
  void tx_result(Person person);

 private:
  suanzi::FaceExtractor  *face_extractor_;

};

}  // namespace suanzi

#endif
