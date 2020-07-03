#ifndef RECOGNIZETIPWIDGET_H
#define RECOGNIZETIPWIDGET_H

#include <QImage>
#include <QWidget>

#include "person.hpp"

namespace suanzi {

class RecognizeTipWidget : public QWidget {
  Q_OBJECT

 public:
  RecognizeTipWidget(QWidget *parent = nullptr);
  ~RecognizeTipWidget() override;

 protected:
  void paintEvent(QPaintEvent *event);

 private slots:
  void rx_display(PersonDisplay person);
  void hide_self();

 private:
  PersonDisplay person_;
};

}  // namespace suanzi

#endif
