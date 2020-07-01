#ifndef DETECTTIPWIDGET_H
#define DETECTTIPWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include "detection_float.h"
#include "pingpangbuffer.h"
#include "image_package.h"

using namespace suanzi;

class DetectTipWidget : public QWidget
{
    Q_OBJECT

public:
    DetectTipWidget(QWidget *parent = nullptr);
    ~DetectTipWidget() override;
    void paint(QPainter *painter);

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void rx_result(PingPangBuffer<ImagePackage> *img, DetectionFloat detection);
    void hide_self();

private:
    QRect rect_;
    bool b_update_;

};

#endif
