#include "recognizetipwidget.h"
#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QDebug>

RecognizeTipWidget::RecognizeTipWidget(QWidget *parent)
    : QWidget(parent)
{
    QPalette palette = this->palette();
    palette.setColor(QPalette::Background, Qt::transparent);
    palette.setColor (QPalette::Foreground, Qt::red);
    setPalette(palette);
}

RecognizeTipWidget::~RecognizeTipWidget()
{

}


void RecognizeTipWidget::rx_result(Person person) {
    person_ = person;
    QTimer::singleShot(3000, this, SLOT(hide_self()));
    show();
}


void RecognizeTipWidget::hide_self() {
    hide();
}


void RecognizeTipWidget::paintEvent(QPaintEvent *event) {

    QPainter painter(this);
    QFont font = painter.font();
    font.setPixelSize(48);
    painter.setFont(font);
    painter.setPen(Qt::red);

    QRect r = rect();
    QFontMetrics metrics = painter.fontMetrics();
    int stringHeight = metrics.ascent() + metrics.descent(); // 不算 line gap
    int stringWidth = metrics.width(QString(person_.name.c_str())); // 字符串的宽度
    int x = r.x() + (r.width() - stringWidth) / 2;
    int y = r.y() + (r.height() - stringHeight) / 2 + metrics.ascent();
    painter.drawText(x, y, QString(person_.name.c_str()));
}
