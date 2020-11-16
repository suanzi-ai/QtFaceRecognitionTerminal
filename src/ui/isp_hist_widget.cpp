#include "isp_hist_widget.hpp"

#include <QFontDatabase>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <tuple>
#include <vector>

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
  setAttribute(Qt::WA_TranslucentBackground);

  font_ = new QFont();
  font_->setPointSize(14);
  font_->setFamily(QFontDatabase::applicationFontFamilies(
                       QFontDatabase::addApplicationFont(":asserts/clock.ttf"))
                       .at(0));

  chart_view_ = new QChartView(chart_);
  // chart_view_->setMinimumSize(width, height);
  // chart_view_->setFixedSize(width, height);
  chart_view_->setStyleSheet("background: transparent");

  chart_->addSeries(series_);

  axis_x_ = new QValueAxis;
  axis_x_->setRange(0, HIST_SIZE);
  axis_x_->setLabelFormat("%g");
  // axis_x_->setTitleText("Hist idx");
  chart_->addAxis(axis_x_, Qt::AlignBottom);
  series_->attachAxis(axis_x_);

  axis_y_ = new QValueAxis;
  axis_y_->setRange(0, HIST_MAX_VALUE);
  // axis_y_->setTitleText("Bright level");
  chart_->addAxis(axis_y_, Qt::AlignLeft);
  series_->attachAxis(axis_y_);

  chart_->layout()->setContentsMargins(0, 0, 0, 0);
  chart_->setContentsMargins(-10, -10, -10, -10);
  chart_->setBackgroundRoundness(0);
  chart_->legend()->hide();
  // chart_->setTitle("Hist");
  chart_->setBackgroundVisible(false);

  label_iso_ = new QLabel("0", this);
  label_iso_calibrate_ = new QLabel("0", this);
  label_ave_lum_ = new QLabel("0", this);
  label_exposure_ = new QLabel("0", this);
  label_exp_time_ = new QLabel("0", this);
  label_short_exp_time_ = new QLabel("0", this);
  label_median_exp_time_ = new QLabel("0", this);
  label_long_exp_time_ = new QLabel("0", this);
  label_a_gain_ = new QLabel("0", this);
  label_d_gain_ = new QLabel("0", this);
  label_isp_d_gain_ = new QLabel("0", this);
  label_lines_per_500ms_ = new QLabel("0", this);

  std::vector<std::tuple<QLabel *, QLabel *>> desc_table = {
      {new QLabel("ISO:", this), label_iso_},
      {new QLabel("ISO calibrate:", this), label_iso_calibrate_},
      {new QLabel("Ave lum:", this), label_ave_lum_},
      {new QLabel("Exposure:", this), label_exposure_},
      {new QLabel("Exp time:", this), label_exp_time_},
      {new QLabel("Short exp time:", this), label_short_exp_time_},
      {new QLabel("Median exp time:", this), label_median_exp_time_},
      {new QLabel("Long exp time:", this), label_long_exp_time_},
      {new QLabel("A gain:", this), label_a_gain_},
      {new QLabel("D gain:", this), label_d_gain_},
      {new QLabel("ISP d gain:", this), label_isp_d_gain_},
      {new QLabel("Lines/500ms:", this), label_lines_per_500ms_},
  };

  main_layout_ = new QGridLayout(this);
  main_layout_->addWidget(chart_view_, 0, 0, desc_table.size(), 8);

  int r = 0;
  QLabel *key, *value;
  for (const auto kv : desc_table) {
    std::tie(key, value) = kv;
    key->setFont(*font_);
    key->setStyleSheet("QLabel { color : white; }");
    value->setFont(*font_);
    value->setStyleSheet("QLabel { color : white; }");

    main_layout_->addWidget(key, r, 8, 1, 3);
    main_layout_->addWidget(value, r, 11, 1, 1);
    r++;
  }

  startTimer(1000);
}

ISPHistWidget::~ISPHistWidget() {
  delete chart_;
  delete series_;
  delete axis_x_;
  delete axis_y_;
  delete chart_view_;
  delete font_;

  delete label_iso_;
  delete label_iso_calibrate_;
  delete label_ave_lum_;
  delete label_exposure_;
  delete label_exp_time_;
  delete label_short_exp_time_;
  delete label_median_exp_time_;
  delete label_long_exp_time_;
  delete label_a_gain_;
  delete label_d_gain_;
  delete label_isp_d_gain_;
  delete label_lines_per_500ms_;

  delete main_layout_;
}

void ISPHistWidget::set_camera_type(io::CameraType camera_type) {
  camera_type_ = camera_type;
}

void ISPHistWidget::timerEvent(QTimerEvent *event) {
  switch (Config::get_app().show_isp_info_window) {
    case ISPInfoWindowBGR:
      set_camera_type(CAMERA_BGR);
      show();
      update();
      break;

    case ISPInfoWindowNIR:
      set_camera_type(CAMERA_NIR);
      show();
      update();
      break;

    default:
      hide();
      return;
  }
}

void ISPHistWidget::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  paint(&painter);
}

void ISPHistWidget::paint(QPainter *painter) {
  static ISPExposureInfo exp_info;
  SZ_RETCODE ret =
      Engine::instance()->isp_query_exposure_info(camera_type_, &exp_info);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("Draw hist, get value failed");
    return;
  }

  auto hist_value = exp_info.hist_1024_value;

  SZ_UINT32 max = 0;
  QVector<QPointF> buffer;
  for (int i = 0; i < HIST_SIZE; i++) {
    buffer.append(QPointF(i, hist_value[i]));

    if (hist_value[i] > max) {
      max = hist_value[i];
    }
  }
  series_->replace(buffer);
  axis_y_->setRange(0, max - max % 1000 + 1000);

  label_iso_->setText(QString::number(exp_info.iso));
  label_iso_calibrate_->setText(QString::number(exp_info.iso_calibrate));
  label_ave_lum_->setText(QString::number(exp_info.ave_lum));
  label_exposure_->setText(QString::number(exp_info.exposure));
  label_exp_time_->setText(QString::number(exp_info.exp_time));
  label_short_exp_time_->setText(QString::number(exp_info.short_exp_time));
  label_median_exp_time_->setText(QString::number(exp_info.median_exp_time));
  label_long_exp_time_->setText(QString::number(exp_info.long_exp_time));
  label_a_gain_->setText(QString::number(exp_info.a_gain));
  label_d_gain_->setText(QString::number(exp_info.d_gain));
  label_isp_d_gain_->setText(QString::number(exp_info.isp_d_gain));
  label_lines_per_500ms_->setText(QString::number(exp_info.lines_per_500ms));
}
