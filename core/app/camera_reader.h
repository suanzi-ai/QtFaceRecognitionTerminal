#ifndef CAMERA_READER_H
#define CAMERA_READER_H

#include <QThread>
#include <QImage>
#include <QSharedPointer>
#include "vi.h"
#include "vpss.h"
#include "vi_vpss.h"
#include "vi_vpss_vo.h"
#include "vo.h"
#include "pingpangbuffer.h"

namespace suanzi {

class CameraReader : QThread {

    Q_OBJECT
 public:
    CameraReader(int cameralIndex, QObject *parent = nullptr);
    ~CameraReader();

 private:
    void run();

 signals:
  void txFrame(PingPangBuffer<MmzImage> *buffer);

 private:
    Vi *pVi_;
    Vpss *pVpss_;
    Vi_Vpss *pVi_Vpss_;
};

}  // namespace suanzi

#endif
