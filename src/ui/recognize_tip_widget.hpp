#ifndef RECOGNIZETIPWIDGET_H
#define RECOGNIZETIPWIDGET_H

#include <QFont>
#include <QPixmap>
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

  QFont font_;
  QPixmap icon_;
  QTimer timer_;

  bool has_info_;
};

}  // namespace suanzi

#endif
