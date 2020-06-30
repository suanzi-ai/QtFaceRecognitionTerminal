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
    QTimer::singleShot(3000, this, SLOT(hideSelf()));
    //if (isVisible())
        //update();
    //else
        show();
}


void RecognizeTipWidget::hideSelf() {
    hide();
}


void RecognizeTipWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawRect(geometry());
    painter.drawText(geometry(), 0, QString(person_.name.c_str()));
}


void RecognizeTipWidget::paint(QPainter *painter) {

    painter->save();

    QFont font = painter->font();
    font.setPixelSize(48);
    painter->setFont(font);
    painter->setPen(Qt::red);
    //painter->setBrush(Qt::green);
    painter->drawRect(geometry());

    painter->drawText(geometry(), 0, QString(person_.name.c_str()));

    painter->restore();
    if (isVisible())
        printf("visible\n");
    else
        printf("invisible\n");
}
