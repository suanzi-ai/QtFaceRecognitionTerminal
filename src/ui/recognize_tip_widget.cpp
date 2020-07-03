#include "recognize_tip_widget.hpp"

#include <QDebug>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

using namespace suanzi;

RecognizeTipWidget::RecognizeTipWidget(QWidget *parent) : QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  palette.setColor(QPalette::Foreground, Qt::red);
  setPalette(palette);
}

RecognizeTipWidget::~RecognizeTipWidget() {}

void RecognizeTipWidget::rx_display(PersonDisplay person) {
  person_ = person;

  if (person_.to_clear) {
    hide();
  } else {
    show();
  }
}

void RecognizeTipWidget::hide_self() { hide(); }

void RecognizeTipWidget::paintEvent(QPaintEvent *event) {
  if (!person_.to_clear) {
    QPainter painter(this);
    QFont font = painter.font();
    font.setPixelSize(48);
    painter.setFont(font);
    painter.setPen(Qt::white);

    QRect r = rect();
    QFontMetrics metrics = painter.fontMetrics();
    int string_height = metrics.ascent() + metrics.descent();
    int string_width = metrics.width(QString(person_.name.c_str()));
    int x = r.x() + (r.width() - string_width) / 2;
    int y = r.height() - 30;

    painter.drawPixmap(rect(), QPixmap(person_.avatar_path.c_str()), QRect());
    painter.drawText(x, y, QString(person_.name.c_str()));
  }
}
