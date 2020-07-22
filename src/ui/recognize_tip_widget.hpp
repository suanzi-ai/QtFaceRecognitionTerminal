#ifndef RECOGNIZETIPWIDGET_H
#define RECOGNIZETIPWIDGET_H

#include <QBitmap>
#include <QImage>
#include <QWidget>

#include "person_service.hpp"

namespace suanzi {

class RecognizeTipWidget : public QWidget {
  Q_OBJECT

 public:
  RecognizeTipWidget(QWidget *parent = nullptr);
  ~RecognizeTipWidget() override;

 protected:
  void paintEvent(QPaintEvent *event);

 private slots:
  void rx_display(PersonData person, bool if_duplicated);
  void rx_reset();

 private:
  PersonData person_;
  QBitmap mask_;
};

}  // namespace suanzi

#endif
