#include "videoplayer.h"
#include "videoframewidget.h"
#include <QBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QDebug>
#include <QTimer>


VideoPlayer::VideoPlayer(QWidget *parent)
    : QWidget(parent)
{
    QPalette pal = palette();
    pal.setColor(QPalette::Background, Qt::transparent);
    pal.setColor (QPalette::Foreground, Qt::green);
    setPalette(pal);

    pDetectTipWidget_ = new DetectTipWidget(this);
    pDetectTipWidget_->hide();
    pRecognizeTipWidget_ = new RecognizeTipWidget();
    pRecognizeTipWidget_->hide();

    pCameralReader1_ = new CameraReader(1, this);
//    pCameralReader0_ = new CameraReader(0, this);

    static QThread detectThread;
    pAliveTask_ = new AliveTask(&detectThread, this);
    pDetectTask_ = new DetectTask(&detectThread, this);
    pRecognizeTask_= new RecognzieTask(this);

//    connect((const QObject *)pCameralReader0_,
//                     SIGNAL(txFrame(PingPangBuffer<MmzImage>*)),
//                    (const QObject *)pAliveTask_,
//                     SLOT(rxFrame(PingPangBuffer<MmzImage>*)));

    connect((const QObject *)pCameralReader1_,
                     SIGNAL(txFrame(PingPangBuffer<ImagePackage>*)),
                    (const QObject *)pDetectTask_,
                     SLOT(rxFrame(PingPangBuffer<ImagePackage>*)));

//    connect((const QObject *)pDetectTask_,
//                     SIGNAL(tx_detection_bgr(PingPangBuffer<ImagePackage> *, DetectionFloat)),
//                    (const QObject *)pRecognizeTask_,
//                     SLOT(rxFrame(PingPangBuffer<MmzImage> *, DetectionFloat)));

//    connect((const QObject *)pDetectTask_,
//                     SIGNAL(tx_detection_bgr(PingPangBuffer<ImagePackage> *, DetectionFloat)),
//                    (const QObject *)pRecognizeTask_,
//                     SLOT(rxFrame(PingPangBuffer<MmzImage> *, DetectionFloat)));


//    connect((const QObject *)pRecognizeTask_,
//                     SIGNAL(txResult(Person)),
//                    (const QObject *)pRecognizeTipWidget_,
//                     SLOT(rxResult(Person)));

   connect((const QObject *)pDetectTask_,
                   SIGNAL(tx_detection_bgr(PingPangBuffer<ImagePackage> *, DetectionFloat)),
                   (const QObject *)pDetectTipWidget_,
                    SLOT(rx_result(PingPangBuffer<ImagePackage> *, DetectionFloat)));

    QTimer::singleShot(1, this, SLOT(init_widgets()));
}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    pDetectTipWidget_->paint(&painter);
    //painter.drawRect(RECOGNIZE_BOX);
}


void VideoPlayer::init_widgets() {
    pRecognizeTipWidget_->setFixedSize(RECOGNIZE_TIP_BOX.width(), RECOGNIZE_TIP_BOX.height());
    pRecognizeTipWidget_->move(RECOGNIZE_TIP_BOX.x(), RECOGNIZE_TIP_BOX.y());
    //pRecognizeTipWidget_->show();
    /*pDetectTipWidget_->setFixedSize(RECOGNIZE_BOX.width(), RECOGNIZE_BOX.height());
    pDetectTipWidget_->move(RECOGNIZE_BOX.x(), RECOGNIZE_BOX.y());
    pDetectTipWidget_->show();*/
}
