#ifndef RECOGNIZETIPWIDGET_H
#define RECOGNIZETIPWIDGET_H

#include <QImage>
#include <QWidget>

#include "person.h"

namespace suanzi {

class RecognizeTipWidget : public QWidget {
  Q_OBJECT

 public:
  RecognizeTipWidget(QWidget *parent = nullptr);
  ~RecognizeTipWidget() override;

 protected:
  void paintEvent(QPaintEvent *event);

 private slots:
  void rx_result(Person person);
  void hide_self();

 private:
  Person person_;
};

}  // namespace suanzi

#endif
