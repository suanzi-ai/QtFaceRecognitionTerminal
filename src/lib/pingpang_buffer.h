#ifndef PINGPANG_BUFFER_H
#define PINGPANG_BUFFER_H

#include <QMetaType>
#include "mmzimage.h"
#include <QThread>

template <class T>
class PingPangBuffer {

 public:
    PingPangBuffer() {

    }

    PingPangBuffer(T *pPingBuffer, T *pPangBuffer) {
        pPing_ = pPingBuffer;
        pPang_ = pPangBuffer;
        b_switch = false;
    }

     T *get_ping() {
         if (b_switch)
             return pPing_;
         else {
             return pPang_;
         }
     }

     void switch_buffer() {
         b_switch = !b_switch;
     }

     T *get_pang() {
         if (b_switch)
             return pPang_;
         else {
             return pPing_;
         }
     }

 private:
    bool b_switch;
    T *pPing_;
    T *pPang_;
};

Q_DECLARE_METATYPE(PingPangBuffer<MmzImage>);

#endif
