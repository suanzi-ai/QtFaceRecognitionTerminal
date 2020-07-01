#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QWidget>
#include <QScopedPointer>
#include "pingpangbuffer.h"
#include "videoframewidget.h"
#include "detecttipwidget.h"
#include "recognizetipwidget.h"
#include "camera_reader.h"
#include "detect_task.h"
#include "recognize_task.h"
#include "alive_task.h"

using namespace suanzi;

class VideoPlayer : public QWidget
{
    Q_OBJECT

public:
    VideoPlayer(QWidget *parent = nullptr);
    ~VideoPlayer();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void init_widgets();

private:
    VideoFrameWidget *pVideoFrameWidget_;
    DetectTipWidget *pDetectTipWidget_;
    RecognizeTipWidget *pRecognizeTipWidget_;

    CameraReader *pCameralReader0_;
    CameraReader *pCameralReader1_;
    AliveTask *pAliveTask_;
    DetectTask *pDetectTask_;
    RecognzieTask *pRecognizeTask_;

    const QRect RECOGNIZE_BOX = {150, 300, 500, 500};
    const QRect RECOGNIZE_TIP_BOX = {150, 200, 500, 100};
};

#endif
