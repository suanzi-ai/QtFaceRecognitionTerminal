#ifndef PINGPANG_BUFFER_H
#define PINGPANG_BUFFER_H

#include <QMetaType>
#include "mmzimage.h"

template <class T>
class PingPangBuffer {

 public:
    PingPangBuffer() {

    }

    PingPangBuffer(T *pPingBuffer, T *pPangBuffer) {
        pPing_ = pPingBuffer;
        pPang_ = pPangBuffer;
        printf("here\n");
    }

     T *getPing() {
         if (pingPangSwitch_.testAndSetOrdered(0, 0))
             return pPing_;
         else {
             return pPang_;
         }
     }

     void switchToPang() {
        pingPangSwitch_.testAndSetOrdered(0, 1);
     }

     void switchToPing() {
        pingPangSwitch_.testAndSetOrdered(1, 0);
     }

     T *getPang() {
         if (pingPangSwitch_.testAndSetOrdered(1, 1))
             return pPing_;
         else {
             return pPang_;
         }
     }

 private:
    bool bPing;
    QAtomicInteger<quint32> pingPangSwitch_;
    T *pPing_;
    T *pPang_;
};

Q_DECLARE_METATYPE(PingPangBuffer<MmzImage>);

#endif
