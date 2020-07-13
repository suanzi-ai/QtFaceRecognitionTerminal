#include "record_task.hpp"

#include <QThread>
#include <chrono>
#include <ctime>
#include <iostream>
#include <string>

#include "logger.hpp"

using namespace suanzi;

RecordTask::RecordTask(PersonService::ptr person_service, MemoryPool<ImageBuffer> *mem_pool, QObject *parent) :
	person_service_(person_service),
	mem_pool_(mem_pool) {
	
    static QThread new_thread;
    moveToThread(&new_thread);
    new_thread.start();
}

RecordTask::~RecordTask() {}


void RecordTask::rx_record(int face_id, ImageBuffer *pBuffer) {

	//todo
	// upload record


	mem_pool_->deallocate(pBuffer);
}
  

