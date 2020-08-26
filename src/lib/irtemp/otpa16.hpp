#ifndef __OTPA16_H
#define __OTPA16_H

#include <QMetaType>

namespace suanzi {

struct OtpaTemperatureData {
	float ambient_temp;
	float pixel_temp[256];
	int max_pixel_temp_index;
};


class Otpa16 {
 public:
  Otpa16();
  ~Otpa16();
  bool read_temperature(OtpaTemperatureData *otpa_temp_data);
  bool set_sample_fps(int sample_interval);

 private:
	int fd_;
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::OtpaTemperatureData);


#endif
