#include "detecttipwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>



DetectTipWidget::DetectTipWidget(QWidget *parent)
    : QWidget(parent)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::transparent);
    palette.setColor (QPalette::Foreground, Qt::green);
    setPalette(palette);
    rect_ = QRect(100, 100, 100, 100);
}

DetectTipWidget::~DetectTipWidget()
{

}


void DetectTipWidget::rx_result(PingPangBuffer<ImagePackage> *img, DetectionFloat detection) {
    int w = 800;
    int h = 1280;
    printf("rx_result\n");
    this->rect_ = QRect(detection.x * w, detection.y * h, detection.width * w, detection.height * h);
    
    QTimer::singleShot(30, this, SLOT(hide_self()));
    show();
}


void DetectTipWidget::hide_self() {
    hide();
}


void DetectTipWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawRect(rect_);
}
