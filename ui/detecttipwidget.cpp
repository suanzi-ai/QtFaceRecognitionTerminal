#include "detecttipwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

DetectTipWidget::DetectTipWidget(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::black);
    setPalette(palette);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
}

DetectTipWidget::~DetectTipWidget()
{

}


void DetectTipWidget::rxResult(QRect &rect) {
    this->rect_ = rect;
    QTimer::singleShot(3000, this, SLOT(hideSelf()));
    update();
}


void DetectTipWidget::hideSelf() {
    hide();
}


void DetectTipWidget::paintEvent(QPaintEvent *event)
{
    /*QPainter painter(this);
    if (image.isNull()) {
        //draw default
        painter.fillRect(event->rect(), palette().background());
    } else {
        painter.drawImage(0, 0, image);
    }*/
}
