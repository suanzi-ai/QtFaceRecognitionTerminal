#include "videoframewidget.h"
#include <QPainter>
#include <QPaintEvent>


VideoFrameWidget::VideoFrameWidget(QWidget *parent)
    : QWidget(parent)
{
#if 0
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::red);
    setPalette(palette);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    setPalette(pal);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
#endif
    pingPang_ = nullptr;
}

VideoFrameWidget::~VideoFrameWidget()
{

}

void VideoFrameWidget::rxVideoFrame(PingPangBuffer<MmzImage> *buffer) {
    MmzImage *pImage = buffer->getPang();
    image = QImage(pImage->pData, pImage->width, pImage->height, QImage::Format_RGB888);
    pingPang_ = buffer;
    printf("rx rgb frame\n");
    static bool bSave = false;
    static int cnt = 0;
    if (!bSave) {
        if (cnt++ > 10) {
            bSave = true;
        }
        image.save(QString::number(cnt) + ".jpg", "JPG", 100);
    }
    update();
}


void VideoFrameWidget::paint(QPainter *painter) {
    if (image.isNull()) {
        //draw default
        //painter->fillRect(geometry(), palette().background());
        //painter->fillRect(geometry(), QBrush(QColor(255, 0, 0)));
        printf("paint here\n");
        painter->drawRect(250, 300, 300, 300);
    } else {
        printf("painter\n");
        //painter->drawImage(0, 0, image);

        if (pingPang_ != nullptr)
            pingPang_->switchToPing();
    }
}
