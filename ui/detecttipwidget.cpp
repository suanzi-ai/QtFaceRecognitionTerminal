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


void DetectTipWidget::rx_result(QRect rect) {
    this->rect_ = rect;
    QTimer::singleShot(3000, this, SLOT(hide_self()));
    update();
}


void DetectTipWidget::hide_self() {
    hide();
}


void DetectTipWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawRect(rect_);
}
