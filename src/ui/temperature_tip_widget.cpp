#include "temperature_tip_widget.hpp"
#include <QHBoxLayout>
#include <QPainter>
#include <QStyleOption>

#include "config.hpp"

using namespace suanzi;

TemperatureTipWidget::TemperatureTipWidget(int screen_width, int screen_height,
                                           int parent_widget_pos_y,
                                           QWidget *parent)
    : QWidget(parent), radius_(0.025 * screen_height) {
  // setAttribute(Qt::WA_StyledBackground, true);
  // setStyleSheet("QWidget {background-color:transparent;color:white;}");
  setWindowFlags(Qt::FramelessWindowHint);
  setAttribute(Qt::WA_TranslucentBackground);

  const int w = screen_width;
  const int h = screen_height;

  move(0.275 * w, parent_widget_pos_y - 0.05578125 * h - 10);

  setFixedSize(0.4875 * w, 0.05578125 * h);

  pl_ok_or_no_ = new QLabel(this);
  pl_ok_or_no_->setFixedSize(0.05625 * w, 0.03515625 * h);

  pl_temperature_ = new QLabel(this);
  pl_temperature_->setAlignment(Qt::AlignTop);
  pl_temperature_->setIndent(5);
  pl_temperature_->setStyleSheet(
      "QLabel {background-color:transparent;color:white;}");

  QFont font;
  font.setPointSize(0.02875 * w);
  pl_temperature_->setFont(font);

  QHBoxLayout *ph_layout = new QHBoxLayout;
  ph_layout->addWidget(pl_ok_or_no_);
  ph_layout->addSpacing(0.025 * w);
  ph_layout->addWidget(pl_temperature_);
  setLayout(ph_layout);
  hide();
}

TemperatureTipWidget::~TemperatureTipWidget() {}

void TemperatureTipWidget::rx_temperature(bool bvisible,
                                          bool bnormal_temperature,
                                          float temperature) {
  if (!bvisible) {
    hide();
    return;
  }

  QString style_str =
      "QLabel {background-color:transparent;border-image: "
      "url(:asserts/%1.png);}";
  if (bnormal_temperature)
    pl_ok_or_no_->setStyleSheet(style_str.arg("tick"));
  else
    pl_ok_or_no_->setStyleSheet(style_str.arg("cross"));

  pl_temperature_->setText(tr("体温") + ":" +
                           QString::number(temperature, 'f', 1) + "°C");

  style_str = "QWidget { background-color:%1; margin:0px;border-radius:%2px; }";

  if (bnormal_temperature)
    setStyleSheet(
        style_str.arg("rgba(0, 100, 0, 180)", QString::number(radius_)));
  else
    setStyleSheet(
        style_str.arg("rgba(220, 0, 0, 150)", QString::number(radius_)));

  show();
}

void TemperatureTipWidget::paintEvent(QPaintEvent *event) {
  QStyleOption opt;
  opt.init(this);
  QPainter painter(this);
  style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
}
