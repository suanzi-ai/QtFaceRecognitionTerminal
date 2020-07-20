#ifndef PINGPANG_BUFFER_H
#define PINGPANG_BUFFER_H

#include <QMetaType>
#include <QThread>

#include "mmzimage.h"

namespace suanzi {

template <class T>
class PingPangBuffer {
 public:
  PingPangBuffer();
  PingPangBuffer(T *ping_buffer, T *pang_buffer);

  T *get_ping();
  T *get_pang();

  void switch_buffer();

 private:
  bool switch_;
  T *ping_;
  T *pang_;
};

}

Q_DECLARE_METATYPE(suanzi::PingPangBuffer<MmzImage>);

#endif
