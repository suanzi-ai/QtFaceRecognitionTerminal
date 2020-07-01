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
        b_write = false;
    }

     T *getPing() {
         //if (pingPangSwitch_.testAndSetOrdered(0, 0))
         if (b_switch)
             return pPing_;
         else {
             return pPang_;
         }
     }

     void startWrite() {
        b_write = true;
     }

     void endWrite() {
        b_write = false;
     }

     void switchToPang() {
        //pingPangSwitch_.testAndSetOrdered(0, 1);
     }

     void switchToPing() {
        //pingPangSwitch_.testAndSetOrdered(1, 0);
         while(b_write) {
            QThread::usleep(1);
         }
         b_switch = !b_switch;


     }

     T *getPang() {
         //if (pingPangSwitch_.testAndSetOrdered(1, 1))
         if (b_switch)
             return pPang_;
         else {
             return pPing_;
         }
     }

 private:
    //QAtomicInteger<quint32> pingPangSwitch_;
    bool b_switch;
    bool b_write;
    T *pPing_;
    T *pPang_;
};

Q_DECLARE_METATYPE(PingPangBuffer<MmzImage>);

#endif
