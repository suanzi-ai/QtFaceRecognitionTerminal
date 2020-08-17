#include "object_temp_task.hpp"
#include "platform_interface.h"

using namespace suanzi;

ObjectTempTask::ObjectTempTask(QObject *parent){
  start();
}

ObjectTempTask::~ObjectTempTask() {}


void ObjectTempTask::rx_ambient_temp(float temp) {
	int ok = Ds_SetEnvTemperature(temp);
	printf("set env temp, %.3f  %d \n", temp, ok);
}


void ObjectTempTask::run() {

	Ds_Initialize();
	Ds_OpenUart("/dev/ttyAMA4");
	while(1) {
		float temp = 0.0, ambient_temp = 0.0;	
		int ok = Ds_GetBodyTemperature(&temp);
		int ok1 = Ds_UpdateEnvTemperature(&ambient_temp);		
		//Ds_SetEnvTemperature(float fEnvTemperature)
		printf("obj ok=%d  object temp=%.3f  env ok=%d  ambient temp=%.3f\n", ok, temp, ok1, ambient_temp);
		usleep(2000000);
	}
}

