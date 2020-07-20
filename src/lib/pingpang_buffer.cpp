#include "pingpang_buffer.hpp"

#include "image_package.h"
#include "recognize_data.hpp"

using namespace suanzi;

template <class T>
PingPangBuffer<T>::PingPangBuffer() {}

template <class T>
PingPangBuffer<T>::PingPangBuffer(T *ping_buffer, T *pang_buffer) {
  ping_ = ping_buffer;
  pang_ = pang_buffer;
  switch_ = false;
}

template <class T>
T *PingPangBuffer<T>::get_ping() {
  if (switch_)
    return ping_;
  else {
    return pang_;
  }
}

template <class T>
T *PingPangBuffer<T>::get_pang() {
  if (switch_)
    return pang_;
  else {
    return ping_;
  }
}

template <class T>
void PingPangBuffer<T>::switch_buffer() { switch_ = !switch_; }

template class PingPangBuffer<ImagePackage>;
template class PingPangBuffer<RecognizeData>;