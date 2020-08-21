#ifndef __OTPA16_H
#define __OTPA16_H

#include <QMetaType>

namespace suanzi {

struct OtpaTempData {
	float ambient_temp;
	float pixel_temp[256];
	float max_pixel_temp_index;
};


class Otpa16 {
 public:
  Otpa16();
  ~Otpa16();
  bool read_temperature(OtpaTempData *otpa_temp_data);
  bool set_sample_fps(int sample_interval);

 private:
	int fd_;
};

}  // namespace suanzi

Q_DECLARE_METATYPE(suanzi::OtpaTempData);


#endif
