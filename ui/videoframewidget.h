#ifndef VIDEOFRAMEWIDGET_H
#define VIDEOFRAMEWIDGET_H

#include <QWidget>
#include <QImage>
#include "pingpangbuffer.h"

class VideoFrameWidget : public QWidget
{
    Q_OBJECT

public:
    VideoFrameWidget(QWidget *parent = nullptr);
    ~VideoFrameWidget() override;
    void paint(QPainter *painter);

protected:
    //void paintEvent(QPaintEvent *event) override;

private slots:
    void rxVideoFrame(PingPangBuffer<MmzImage> *buffer);

private:
    QImage image;
    PingPangBuffer<MmzImage> *pingPang_;
};

#endif
