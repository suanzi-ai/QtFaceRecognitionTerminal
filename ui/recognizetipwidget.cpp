#include "recognizetipwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>

RecognizeTipWidget::RecognizeTipWidget(QWidget *parent)
    : QWidget(parent) {
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::transparent);
    setPalette(palette);
}

RecognizeTipWidget::~RecognizeTipWidget() {

}


void RecognizeTipWidget::rxResult(Person person) {
    person_ = person;
    QTimer::singleShot(3000, this, SLOT(hideSelf()));
    show();
}


void RecognizeTipWidget::hideSelf() {
    hide();
}


void RecognizeTipWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    QFont font = painter.font();
    font.setPixelSize(48);
    painter.setFont(font);
    painter.setPen(Qt::green);
    painter.drawText(geometry(), 0, QString(person_.name.c_str()));
}
