#ifndef ISP_HIST_WIDGET_H
#define ISP_HIST_WIDGET_H

#include <QImage>
#include <QPainter>
#include <QWidget>

#define HIST_SIZE 1024
#define HIST_MAX_VALUE 2048

namespace suanzi {

class ISPHistWidget : public QWidget {
  Q_OBJECT

 public:
  ISPHistWidget(QWidget *parent = nullptr);
  ~ISPHistWidget() override;

  void paint(QPainter *painter);

 private slots:

 private:
};

}  // namespace suanzi

#endif
