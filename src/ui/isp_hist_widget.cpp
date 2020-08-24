#include "isp_hist_widget.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#include "config.hpp"
#include "isp.h"

using namespace suanzi;

ISPHistWidget::ISPHistWidget(QWidget *parent) : QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);
}

ISPHistWidget::~ISPHistWidget() {}

void ISPHistWidget::paint(QPainter *painter) {
  static ISP_EXP_INFO_S exp_info;
  if (!Isp::getInstance()->query_exposure_info(0, &exp_info)) {
    SZ_LOG_ERROR("Draw hist, get value failed");
    return;
  }

  auto hist_value = exp_info.au32AE_Hist1024Value;

  QImage hist(HIST_SIZE, HIST_MAX_VALUE, QImage::Format_RGB888);
  {
    QPainter img_painter(&hist);

    QRect rect = {0, 0, hist.width(), hist.height()};
    img_painter.fillRect(rect, Qt::black);

    img_painter.setPen(Qt::white);
    for (int i; i < HIST_SIZE; i++) {
      auto v = hist_value[i] < HIST_MAX_VALUE ? hist_value[i] : HIST_MAX_VALUE;
      img_painter.drawLine(HIST_SIZE - i - 1, HIST_MAX_VALUE - 1,
                           HIST_SIZE - i - 1, HIST_MAX_VALUE - v - 1);
    }
  }

  // QPainter painter(this);
  hist.scaled(width(), height());
  QRect hist_rect(0, 0, width(), height());
  painter->drawImage(hist_rect, hist);
  painter->setPen(Qt::white);
  painter->drawRect(hist_rect);
}
