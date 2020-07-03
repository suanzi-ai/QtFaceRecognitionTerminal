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

void RecognizeTipWidget::rx_result(Person person) {
  person_ = person;
  QTimer::singleShot(3000, this, SLOT(hide_self()));
  show();
}

void RecognizeTipWidget::hide_self() { hide(); }

void RecognizeTipWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  QFont font = painter.font();
  font.setPixelSize(48);
  painter.setFont(font);
  painter.setPen(Qt::red);
  person_.name = "hello";
  QRect r = rect();
  QFontMetrics metrics = painter.fontMetrics();
  int string_height = metrics.ascent() + metrics.descent();  // 不算 line gap
  int string_width =
      metrics.width(QString(person_.name.c_str()));  // 字符串的宽度
  int x = r.x() + (r.width() - string_width) / 2;
  int y = r.y() + (r.height() - string_height) / 2 + metrics.ascent();
  

  painter.drawPixmap(rect(), QPixmap("avatar_unknown.jpg"), QRect());
  painter.drawText(x, y, QString(person_.name.c_str()));
}
