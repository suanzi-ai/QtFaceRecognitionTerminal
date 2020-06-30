#include "camera_reader.h"
#include "vi.h"
#include "sz_common.h"

using namespace suanzi;

CameraReader::CameraReader(int cameralIndex, QObject *parent) {

    if (cameralIndex == 1) {
        pVi_ = new Vi(cameralIndex, 2, SONY_IMX307_MIPI_2M_30FPS_12BIT);
    } else {
        pVi_ = new Vi(cameralIndex, cameralIndex, SONY_IMX307_MIPI_2M_30FPS_12BIT);
    }

    pVpss_ = new Vpss(cameralIndex, 1920, 1080);

    SZ_SIZE vpssChSizes[2] = {{1920, 1080}, {256, 256}};
    int chIndexes[2] = {0, 1};
    bool rotates[2] = {false, false};
    pVi_Vpss_ = new Vi_Vpss(pVi_, pVpss_, vpssChSizes, chIndexes, rotates, 2);

    static Vo vo(0, VO_INTF_MIPI, 800, 1280);
    static Vi_Vpss_Vo vi_vpss_vo(pVi_Vpss_, &vo);

    start();
}


CameraReader::~CameraReader() {
    delete pVi_;
    delete pVpss_;
    delete pVi_Vpss_;
}


void CameraReader::run() {

    MmzImage ping(256, 256, SZ_IMAGETYPE_NV21);
    MmzImage pang(256, 256, SZ_IMAGETYPE_NV21);
    PingPangBuffer<MmzImage> pingpang(&ping, &pang);
    while(1) {
        MmzImage *pPing = pingpang.getPing();
        if (pVpss_->getYuvFrame(pPing, 1)) {
            pingpang.switchToPang();
            emit txFrame(&pingpang);
            printf("tx0 threadId=%x   %x\n", QThread::currentThreadId(), pPing);
        } else {
            QThread::msleep(1);
        }
    }
}
