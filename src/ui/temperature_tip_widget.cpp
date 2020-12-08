#include "temperature_tip_widget.hpp"

#include <QHBoxLayout>

using namespace suanzi;

TemperatureTipWidget::TemperatureTipWidget(int screen_width, int screen_height,
                                           QWidget *parent)
    : QWidget(parent) {
  setAttribute(Qt::WA_StyledBackground, true);
  setStyleSheet(
      "QWidget {background-color:rgba(5, 0, 20, "
      "150);margin:0px;border-radius:10px;} QLabel "
      "{background-color:transparent;color:white;}");
  move(0.275 * screen_width, 0.11796875 * screen_height);
  setFixedSize(0.4875 * screen_width, 0.05078125 * screen_height);

  pl_ok_or_no_ = new QLabel(this);
  pl_ok_or_no_->setFixedSize(0.025 * screen_width, 0.021875 * screen_height);
  pl_temperature_ = new QLabel(this);
  pl_temperature_->setAlignment(Qt::AlignTop);
  pl_temperature_->setIndent(5);
  pl_temperature_->setStyleSheet(
      "QLabel {background-color:transparent;color:white;}");

  QFont font;
  font.setPointSize(0.02875 * screen_width);
  pl_temperature_->setFont(font);

  QHBoxLayout *ph_layout = new QHBoxLayout;
  ph_layout->addWidget(pl_ok_or_no_);
  ph_layout->addWidget(pl_temperature_);
  setLayout(ph_layout);
}

TemperatureTipWidget::~TemperatureTipWidget() {}

void TemperatureTipWidget::rx_temperature(bool bvisible,
                                          bool bnormal_temperature,
                                          float temperature) {
  if (!bvisible) {
    hide();
    return;
  }

  QString style_str = "QLabel {border-image: url(:asserts/cross.png);}";
  if (bnormal_temperature)
    style_str = "QLabel {border-image: url(:asserts/tick.png);}";
  pl_ok_or_no_->setStyleSheet(style_str);
  pl_temperature_->setText(tr("体温") + ":" +
                           QString::number(temperature, 'f', 1) + "°C");

  //
  style_str =
      "QWidget {background-color:rgba(220, 0, 0, "
      "150);margin:0px;border-radius:10px;} QLabel "
      "{background-color:transparent;}";
  if (bnormal_temperature)
    style_str =
        "QWidget {background-color:rgba(0, 100, 0, "
        "180);margin:0px;border-radius:10px;} QLabel "
        "{background-color:transparent;}";
  setStyleSheet(style_str);

  show();
}
