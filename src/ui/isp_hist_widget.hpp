#ifndef ISP_HIST_WIDGET_H
#define ISP_HIST_WIDGET_H

#include <QGraphicsLayout>
#include <QImage>
#include <QPainter>
#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtWidgets/QVBoxLayout>
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
  ISPHistWidget(int width, int height,
                QWidget *parent = nullptr);
  ~ISPHistWidget() override;

  void set_camera_type(io::CameraType camera_type);

  void paint(QPainter *painter);
  void paint_desc(QPainter *painter, const ISPExposureInfo &exp_info);

 private slots:

 private:
  io::CameraType camera_type_ = io::CAMERA_BGR;
  QFont font_;
  QChart *chart_;
  QLineSeries *series_;

  QChartView *chart_view_;
  QValueAxis *axis_x_;
  QValueAxis *axis_y_;
  QVBoxLayout *main_layout_;
};

}  // namespace suanzi

#endif
