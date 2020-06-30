#include "videoplayer.h"
#include "videoframewidget.h"
#include <QBoxLayout>
#include <QPainter>
#include <QPushButton>


VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::transparent);
    pal.setColor (QPalette::Foreground, Qt::green);
    setPalette(pal);

    pDetectTipWidget_ = new DetectTipWidget();
    pDetectTipWidget_->hide();
    pRecognizeTipWidget_ = new RecognizeTipWidget();
    //pRecognizeTipWidget_->setFixedSize(500, 100);
    pRecognizeTipWidget_->setGeometry(150, 200, 500, 100);
    pRecognizeTipWidget_->hide();



    pCameralReader1_ = new CameraReader(1, this);
    pCameralReader0_ = new CameraReader(0, this);

    static QThread detectThread;
    pAliveTask_ = new AliveTask(&detectThread, this);
    pDetectTask_ = new DetectTask(&detectThread, this);
    pRecognizeTask_= new RecognzieTask(this);

    connect((const QObject *)pCameralReader0_,
                     SIGNAL(txFrame(PingPangBuffer<ImagePackage>*)),
                    (const QObject *)pAliveTask_,
                     SLOT(rxFrame(PingPangBuffer<ImagePackage>*)));

    connect((const QObject *)pCameralReader1_,
                     SIGNAL(txFrame(PingPangBuffer<ImagePackage>*)),
                    (const QObject *)pDetectTask_,
                     SLOT(rxFrame(PingPangBuffer<ImagePackage>*)));

    connect((const QObject *)pDetectTask_,
                     SIGNAL(txFrame(PingPangBuffer<MmzImage>*)),
                    (const QObject *)pRecognizeTask_,
                     SLOT(rxFrame(PingPangBuffer<MmzImage>*)));
    connect((const QObject *)pRecognizeTask_,
                     SIGNAL(txResult(Person)),
                    (const QObject *)pRecognizeTipWidget_,
                     SLOT(rxResult(Person)));

#if 0
    //pVideoFrameWidget_ = new VideoFrameWidget(this);
    QBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(pRecognizeTipWidget_);
    layout->setAlignment(Qt::AlignCenter);
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
    //if (pRecognizeTipWidget_->isVisible())
    //QPainter painter1(pRecognizeTipWidget_);
    painter.drawRect(150, 300, 500, 500);
}

