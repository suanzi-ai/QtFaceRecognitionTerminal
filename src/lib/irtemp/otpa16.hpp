#ifndef __OTPA16_H
#define __OTPA16_H


namespace suanzi {

class Otpa16 {
 public:
  Otpa16();
  ~Otpa16();
  bool read_temperature(unsigned char *buf, int len);
  bool set_sample_fps(int sample_interval);

 private:
	int fd_;
};

}  // namespace suanzi

#endif
