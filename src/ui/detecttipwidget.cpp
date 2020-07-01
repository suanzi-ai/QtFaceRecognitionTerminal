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
    b_update_ = false;
}

DetectTipWidget::~DetectTipWidget()
{

}

void DetectTipWidget::paint(QPainter *painter) {
    if (b_update_)
        painter->drawRect(rect_);
}

void DetectTipWidget::rx_result(PingPangBuffer<ImagePackage> *img, DetectionFloat detection) {
    int w = 800;
    int h = 1280;
    b_update_ = detection.b_valid;
    if (b_update_)
        this->rect_ = QRect(detection.x * w, detection.y * h, detection.width * w, detection.height * h);
    
    //QTimer::singleShot(25, this, SLOT(hide_self()));
    //show();
    QWidget *p = (QWidget *)parent();
    p->update();
}


void DetectTipWidget::hide_self() {
    //hide();
}


void DetectTipWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawRect(rect_);
}
