#ifndef RECORD_TASK_H
#define RECORD_TASK_H

#include <QObject>

#include "memory_pool.hpp"
#include "mmzimage.h"
#include "person_service.hpp"


namespace suanzi {

class RecordTask : QObject {
  Q_OBJECT
 public:
  RecordTask(PersonService::ptr person_service, MemoryPool<ImageBuffer> *mem_pool, QObject *parent = nullptr);
  ~RecordTask();

 private slots:
  void rx_record(int face_id, ImageBuffer *pBuffer);
  

 private:
  suanzi::PersonService::ptr person_service_;
  MemoryPool<ImageBuffer> *mem_pool_;
};


}  // namespace suanzi

#endif
