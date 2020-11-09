#include "isp_hist_widget.hpp"

#include <QFontDatabase>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QVBoxLayout>
#include <quface-io/engine.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

QT_CHARTS_USE_NAMESPACE

ISPHistWidget::ISPHistWidget(int width, int height, QWidget *parent)
    : QWidget(parent), chart_(new QChart), series_(new QLineSeries) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);
  setFixedSize(width, height);
  move(0, 50);
  setAttribute(Qt::WA_TranslucentBackground);

  QChartView *chartView = new QChartView(chart_);
  // chartView->setMinimumSize(width, height);
  chartView->setFixedSize(width, height);
  chartView->setStyleSheet("background: transparent");

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
  chart_->setBackgroundVisible(false);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(chartView);

  font_.setFamily(QFontDatabase::applicationFontFamilies(
                      QFontDatabase::addApplicationFont(":asserts/clock.ttf"))
                      .at(0));
}

ISPHistWidget::~ISPHistWidget() {}

void ISPHistWidget::paint(QPainter *painter) {
  static ISPExposureInfo exp_info;
  SZ_RETCODE ret =
      Engine::instance()->isp_query_exposure_info(CAMERA_BGR, &exp_info);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Draw hist, get value failed");
    return;
  }

  auto hist_value = exp_info.hist_1024_value;

  QVector<QPointF> buffer;
  for (int i = 0; i < HIST_SIZE; i++) {
    buffer.append(QPointF(i, hist_value[i]));
  }
  series_->replace(buffer);

  QString desc[] = {
      QString::asprintf("ISO:--------------%d\n", exp_info.iso),
      QString::asprintf("ISO calibrate:----%d\n", exp_info.iso_calibrate),
      QString::asprintf("Ave lum:----------%d\n", exp_info.ave_lum),
      QString::asprintf("Exposure:---------%d\n", exp_info.exposure),
      QString::asprintf("Exp time:---------%d\n", exp_info.exp_time),
      QString::asprintf("Short exp time:---%d\n", exp_info.short_exp_time),
      QString::asprintf("Median exp time:--%d\n", exp_info.median_exp_time),
      QString::asprintf("Long exp time:----%d\n", exp_info.long_exp_time),
      QString::asprintf("A gain:-----------%d\n", exp_info.a_gain),
      QString::asprintf("D gain:-----------%d\n", exp_info.d_gain),
      QString::asprintf("ISP d gain:-------%d\n", exp_info.isp_d_gain),
      QString::asprintf("Lines per 500ms:--%d\n", exp_info.lines_per_500ms),
  };

  QFont font_backup = painter->font();

  painter->setFont(font_);

  int x_offset = width() + 10;
  int y_offset = 50;
  for (auto &d : desc) {
    painter->drawText(x() + x_offset, y() + y_offset, d);
    y_offset += 20;
  }

  painter->setFont(font_backup);
}
