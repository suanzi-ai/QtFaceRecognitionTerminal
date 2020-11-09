#ifndef ISP_HIST_WIDGET_H
#define ISP_HIST_WIDGET_H

#include <QGraphicsLayout>
#include <QImage>
#include <QPainter>
#include <QWidget>
#include <QtCharts/QChart>

QT_CHARTS_BEGIN_NAMESPACE
class QLineSeries;
class QChart;
QT_CHARTS_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

#define HIST_SIZE 1024
#define HIST_MAX_VALUE 4096

namespace suanzi {

class ISPHistWidget : public QWidget {
  Q_OBJECT

 public:
  ISPHistWidget(int width, int height, QWidget *parent = nullptr);
  ~ISPHistWidget() override;

  void paint(QPainter *painter);

 private slots:

 private:
  QFont font_;
  QChart *chart_;
  QLineSeries *series_;
};

}  // namespace suanzi

#endif
