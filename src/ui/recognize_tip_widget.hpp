#ifndef RECOGNIZETIPWIDGET_H
#define RECOGNIZETIPWIDGET_H

#include <QBitmap>
#include <QImage>
#include <QTimer>
#include <QWidget>

#include "person_service.hpp"

namespace suanzi {

class RecognizeTipWidget : public QWidget {
  Q_OBJECT

 public:
  RecognizeTipWidget(int width, int height, QWidget *parent = nullptr);
  ~RecognizeTipWidget() override;

  void paint(QPainter *painter);

 private slots:
  void rx_display(PersonData person, bool if_duplicated);
  void rx_reset();

 private:
  PersonData person_;

  QTimer timer_;
};

}  // namespace suanzi

#endif
