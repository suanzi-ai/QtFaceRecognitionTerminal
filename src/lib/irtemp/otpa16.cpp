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
	fd_ = open("/dev/i2c-device3", O_RDWR);
    if (fd_ < 0) {
        SZ_LOG_ERROR("can't open /dev/i2c-device3");
        return;
    }
}

Otpa16::~Otpa16() {close(fd_);}


bool Otpa16::read_temperature(unsigned char *buf, int len) {
	if (len < 5)
		return false;
	buf[0] = 4;//cmd len
	buf[1] = 0x68; //device addr
	buf[2] = 0xd0; //register addr
	buf[3] = 0x4e;
	buf[4] = 0x00;
	return read(fd_, buf, len) == len;
}


bool Otpa16::set_sample_fps(int sample_interval) {
	unsigned char buf[5];
	buf[0] = 0x68;//device addr
	buf[1] = 0xd0;//register addr
	buf[2] = 0x2f;
	buf[3] = 0x06;//default 1fps
	switch(sample_interval) {
	  case 250:
	  buf[3] = 0x04; //4fps
	  break;
	  case 500:
	  buf[3] = 0x05; //2fps
	  break;
	  case 1000:
	  buf[3] = 0x06;//1fps
	  break;
	  case 2000:
	  buf[3] = 0x07;//0.5fps
	  break;
    }
	buf[4] = 0x00;
    return write(fd_, buf, 5) == 1;
}
