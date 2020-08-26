#include "otpa16.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "logger.hpp"

using namespace suanzi;

Otpa16::Otpa16() {
	qRegisterMetaType<OtpaTemperatureData>("OtpaTemperatureData");
	fd_ = open("/dev/i2c-device3", O_RDWR);
    if (fd_ < 0) {
        SZ_LOG_ERROR("can't open /dev/i2c-device3");
        return;
    }
}

Otpa16::~Otpa16() {close(fd_);}


bool Otpa16::read_temperature(OtpaTemperatureData *otpa_temp_data) {
	
	unsigned char buf[525];
	buf[0] = 2;//cmd len
	buf[1] = 0x4e;
	buf[2] = 0x00;
	if (read(fd_, buf, 525) != 525)
		return false;
	
	//check data is valid
	if (buf[0] != 0x02 || buf[1] != 0x4e || buf[8] != 0x03)
		return false;
	
	static unsigned char update_seq_no = 255;
	unsigned char cur_update_seq_no = buf[3] & 0x0f;
	//read old data
	if (update_seq_no == cur_update_seq_no)
		return false;
	update_seq_no = cur_update_seq_no;
	
	otpa_temp_data->ambient_temp = (buf[9] * 256 + buf[10] - 27315) / 100.0;				
	float max_temp = 0.0;
	int offset = 0;
	for (int i = 13, offset = 0; i < 525; i += 2, offset++) { 
	
		otpa_temp_data->pixel_temp[offset] = (buf[i] * 256 + buf[i + 1] - 27315) / 100.0;	
		if (max_temp < otpa_temp_data->pixel_temp[offset]) {
			max_temp = otpa_temp_data->pixel_temp[offset];
			otpa_temp_data->max_pixel_temp_index = offset;
		}
	}
	return true;
}


bool Otpa16::set_sample_fps(int sample_interval) {
	unsigned char buf[3];
	buf[0] = 0x2f;
	buf[1] = 0x06;//default 1fps
	switch(sample_interval) {
	  case 250:
	  buf[1] = 0x04; //4fps
	  break;
	  case 500:
	  buf[1] = 0x05; //2fps
	  break;
	  case 1000:
	  buf[1] = 0x06;//1fps
	  break;
	  case 2000:
	  buf[1] = 0x07;//0.5fps
	  break;
    }
	buf[2] = 0x00;
    return write(fd_, buf, 3) == 1;
}
