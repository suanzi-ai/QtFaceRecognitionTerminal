#ifndef ISP_HIST_WIDGET_H
#define ISP_HIST_WIDGET_H

#include <QGraphicsLayout>
#include <QImage>
#include <QLabel>
#include <QPainter>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QGridLayout>
#include <quface-io/engine.hpp>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

#define HIST_SIZE 1023
#define HIST_MAX_VALUE 4096

namespace suanzi {

class ISPHistWidget : public QWidget {
  Q_OBJECT

 public:
  ISPHistWidget(int width, int height, QWidget* parent = nullptr);
  ~ISPHistWidget() override;

  void set_camera_type(io::CameraType camera_type);

 protected:
  void timerEvent(QTimerEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

 private:
  void paint(QPainter* painter);

 private:
  io::CameraType camera_type_ = io::CAMERA_BGR;

  QGridLayout* main_layout_;

  QFont* font_;
  QChart* chart_;
  QLineSeries* series_;

  QChartView* chart_view_;
  QValueAxis* axis_x_;
  QValueAxis* axis_y_;

  QLabel* label_iso_;
  QLabel* label_iso_calibrate_;
  QLabel* label_ave_lum_;
  QLabel* label_exposure_;
  QLabel* label_exp_time_;
  QLabel* label_short_exp_time_;
  QLabel* label_median_exp_time_;
  QLabel* label_long_exp_time_;
  QLabel* label_a_gain_;
  QLabel* label_d_gain_;
  QLabel* label_isp_d_gain_;
  QLabel* label_lines_per_500ms_;
};

}  // namespace suanzi

#endif
