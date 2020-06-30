#include "recognizetipwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

RecognizeTipWidget::RecognizeTipWidget(QWidget *parent)
    : QWidget(parent)
{
    /*setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground, true);

    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::red);
    //pal.setColor(QPalette::Background, QColor(0x00,0xff,0x00,0x00));
    palette.setColor (QPalette::Foreground, Qt::red);
    setPalette(palette);

    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);*/
}

RecognizeTipWidget::~RecognizeTipWidget()
{

}


void RecognizeTipWidget::rxResult(Person person) {
    person_ = person;
    printf("rxResult person\n");
    //QTimer::singleShot(3000, this, SLOT(hideSelf()));
    if (isVisible())
        update();
    else
        show();
}


void RecognizeTipWidget::hideSelf() {
    hide();
}


void RecognizeTipWidget::paint(QPainter *painter) {

    painter->setPen(Qt::gray);
    painter->setBrush(Qt::green);
    painter->drawRect(10,10,200,200);
    painter->drawText(0, 0, "hello");
    printf("hahaaaa\n");

}
