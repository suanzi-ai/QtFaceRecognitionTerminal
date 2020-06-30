#include "videoplayer.h"
#include "videoframewidget.h"
#include <QBoxLayout>
#include <QPainter>
#include <QPushButton>


VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    pal.setColor (QPalette::Foreground, QColor(0,255,0,255));
    setPalette(pal);

    pDetectTipWidget_ = new DetectTipWidget(this);
    pDetectTipWidget_->hide();
    pRecognizeTipWidget_ = new RecognizeTipWidget(this);
    //pRecognizeTipWidget_->hide();



    pCameralReader1_ = new CameraReader(1, this);
    pCameralReader0_ = new CameraReader(0, this);

    static QThread detectThread;
    pAliveTask_ = new AliveTask(&detectThread, this);
    pDetectTask_ = new DetectTask(&detectThread, this);
    pRecognizeTask_= new RecognzieTask(this);

    connect((const QObject *)pCameralReader0_,
                     SIGNAL(txFrame(PingPangBuffer<MmzImage>*)),
                    (const QObject *)pAliveTask_,
                     SLOT(rxFrame(PingPangBuffer<MmzImage>*)));

    connect((const QObject *)pCameralReader1_,
                     SIGNAL(txFrame(PingPangBuffer<MmzImage>*)),
                    (const QObject *)pDetectTask_,
                     SLOT(rxFrame(PingPangBuffer<MmzImage>*)));

    connect((const QObject *)pDetectTask_,
                     SIGNAL(txFrame(PingPangBuffer<MmzImage>*)),
                    (const QObject *)pRecognizeTask_,
                     SLOT(rxFrame(PingPangBuffer<MmzImage>*)));
    connect((const QObject *)pRecognizeTask_,
                     SIGNAL(txResult(Person)),
                    (const QObject *)pRecognizeTipWidget_,
                     SLOT(rxResult(Person)));


#if 1
    //pVideoFrameWidget_ = new VideoFrameWidget(this);
    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(pRecognizeTipWidget_);
    setLayout(layout);
//    connect((const QObject *)pCameralReader,
//                     SIGNAL(txVideoFrame(PingPangBuffer<MmzImage>*)),
//                     pVideoFrameWidget_,
//                     SLOT(rxVideoFrame(PingPangBuffer<MmzImage>*)));
#endif
    //setGeometry()
}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    //pVideoFrameWidget_->paint(&painter);
    pRecognizeTipWidget_->paint(&painter);
   // painter.drawRect(150, 300, 500, 500);
}

