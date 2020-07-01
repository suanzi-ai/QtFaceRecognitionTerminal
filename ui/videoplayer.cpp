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

    pDetectTipWidget_ = new DetectTipWidget();
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

    connect((const QObject *)pDetectTask_,
                     SIGNAL(txFrame(PingPangBuffer<MmzImage>*)),
                    (const QObject *)pRecognizeTask_,
                     SLOT(rxFrame(PingPangBuffer<MmzImage>*)));
    connect((const QObject *)pRecognizeTask_,
                     SIGNAL(txResult(Person)),
                    (const QObject *)pRecognizeTipWidget_,
                     SLOT(rxResult(Person)));

    QTimer::singleShot(1, this, SLOT(initWidgets()));
}

VideoPlayer::~VideoPlayer()
{

}

void VideoPlayer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawRect(150, 300, 500, 500);
}


void VideoPlayer::initWidgets() {
    pRecognizeTipWidget_->setFixedSize(500, 100);
    pRecognizeTipWidget_->move(150, 200);
    pRecognizeTipWidget_->show();
}
