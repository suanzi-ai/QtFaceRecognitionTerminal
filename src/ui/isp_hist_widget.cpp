#include "isp_hist_widget.hpp"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QVBoxLayout>

#include "config.hpp"
#include "isp.h"

using namespace suanzi;

QT_CHARTS_USE_NAMESPACE

ISPHistWidget::ISPHistWidget(QWidget *parent)
    : QWidget(parent), chart_(new QChart), series_(new QLineSeries) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  QChartView *chartView = new QChartView(chart_);
  chartView->setMinimumSize(size());
  chart_->addSeries(series_);

  QValueAxis *axisX = new QValueAxis;
  axisX->setRange(0, HIST_SIZE);
  axisX->setLabelFormat("%g");
  // axisX->setTitleText("Hist idx");
  chart_->addAxis(axisX, Qt::AlignBottom);
  series_->attachAxis(axisX);

  QValueAxis *axisY = new QValueAxis;
  axisY->setRange(0, HIST_MAX_VALUE);
  // axisY->setTitleText("Bright level");
  chart_->addAxis(axisY, Qt::AlignLeft);
  series_->attachAxis(axisY);

  chart_->layout()->setContentsMargins(0, 0, 0, 0);
  chart_->setContentsMargins(-10, -10, -10, -10);
  chart_->setBackgroundRoundness(0);
  chart_->legend()->hide();
  // chart_->setTitle("Hist");

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(chartView);
}

ISPHistWidget::~ISPHistWidget() {}

void ISPHistWidget::paint(QPainter *painter) {
  static ISP_EXP_INFO_S exp_info;
  if (!Isp::getInstance()->query_exposure_info(0, &exp_info)) {
    SZ_LOG_ERROR("Draw hist, get value failed");
    return;
  }

  auto hist_value = exp_info.au32AE_Hist1024Value;

  QVector<QPointF> buffer;
  for (int i = 0; i < HIST_SIZE; i++) {
    buffer.append(QPointF(i, hist_value[i]));
  }
  series_->replace(buffer);
}
