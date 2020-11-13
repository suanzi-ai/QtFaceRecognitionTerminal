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

  chart_view_ = new QChartView(chart_);
  // chart_view_->setMinimumSize(width, height);
  chart_view_->setFixedSize(width, height);
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

  main_layout_ = new QVBoxLayout(this);
  main_layout_->addWidget(chart_view_);

  font_.setFamily(QFontDatabase::applicationFontFamilies(
                      QFontDatabase::addApplicationFont(":asserts/clock.ttf"))
                      .at(0));
}

ISPHistWidget::~ISPHistWidget() {
  delete chart_;
  delete series_;
  delete axis_x_;
  delete axis_y_;
  delete chart_view_;
  delete main_layout_;
}

void ISPHistWidget::paint_desc(QPainter *painter,
                               const ISPExposureInfo &exp_info) {
  std::vector<std::tuple<QString, QString>> desc = {
      {"ISO:", QString::asprintf("%d", exp_info.iso)},
      {"ISO calibrate:", QString::asprintf("%d", exp_info.iso_calibrate)},
      {"Ave lum:", QString::asprintf("%d", exp_info.ave_lum)},
      {"Exposure:", QString::asprintf("%d", exp_info.exposure)},
      {"Exp time:", QString::asprintf("%d", exp_info.exp_time)},
      {"Short exp time:", QString::asprintf("%d", exp_info.short_exp_time)},
      {"Median exp time:", QString::asprintf("%d", exp_info.median_exp_time)},
      {"Long exp time:", QString::asprintf("%d", exp_info.long_exp_time)},
      {"A gain:", QString::asprintf("%d", exp_info.a_gain)},
      {"D gain:", QString::asprintf("%d", exp_info.d_gain)},
      {"ISP d gain:", QString::asprintf("%d", exp_info.isp_d_gain)},
      {"Lines/500ms:", QString::asprintf("%d", exp_info.lines_per_500ms)},
  };

  int x_offset = width() + 10;
  int y_offset = 50;

  QString key, value;
  for (const auto kv : desc) {
    std::tie(key, value) = kv;
    painter->drawText(x() + x_offset, y() + y_offset, key);
    painter->drawText(x() + x_offset + 140, y() + y_offset, value);
    y_offset += 20;
  }
}

void ISPHistWidget::set_camera_type(io::CameraType camera_type) {
  camera_type_ = camera_type;
}

void ISPHistWidget::paint(QPainter *painter) {
  switch (Config::get_app().show_isp_info_window) {
    case ISPInfoWindowBGR:
      set_camera_type(CAMERA_BGR);
      show();
      break;

    case ISPInfoWindowNIR:
      set_camera_type(CAMERA_NIR);
      show();
      break;

    default:
      hide();
      return;
  }

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

  QFont font_backup = painter->font();

  painter->setFont(font_);

  paint_desc(painter, exp_info);

  painter->setFont(font_backup);
}
