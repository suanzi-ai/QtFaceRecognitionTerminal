#ifndef OUTLINE_WIDGET_H
#define OUTLINE_WIDGET_H

#include <QLabel>
#include <QPainter>
#include <QTimer>
#include <QWidget>

namespace suanzi {

class TemperatureTipLabel : public QLabel {
  Q_OBJECT
 public:
  TemperatureTipLabel(int screen_width, int screen_height,
                      QWidget *parent = 0) {
    temperature_ = 0.0;
    setFixedSize(240, 165);
    move(screen_width / 2 - 120, 0.24 * screen_height);
    setStyleSheet("QLabel { background-color:transparent; color:blue; }");
    hide();
    QTimer::singleShot(1, this, SLOT(delay_init_widgets()));
  }

  void set_temperature(float temperature) {
    temperature_ = temperature;
    update();
  }

 private slots:
  void delay_init_widgets() { show(); }

 private:
  void paintEvent(QPaintEvent *event) override {
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int arc_width = 240;
    int up_arc_height = 170;
    int down_arc_height = 80;

    // draw up arc
    QRectF rect(0, 0, arc_width, up_arc_height);
    QPainterPath path;
    path.moveTo(arc_width / 2, up_arc_height / 2);
    path.arcTo(rect, 0, 180);
    // painter.setBrush(QBrush(QColor(255, 255, 255, 125)));
    painter.setPen(Qt::NoPen);
    // painter.drawPath(path);
    painter.fillPath(path, QBrush(QColor(255, 255, 255, 10)));

    // draw down arc
    rect.setRect(0, (up_arc_height - down_arc_height) / 2, arc_width,
                 down_arc_height);
    path.arcTo(rect, 180, 180);
    painter.fillPath(path, QBrush(QColor(255, 255, 255, 150)));
    // painter.drawPath(path);

    // draw temperature
    QFont font = painter.font();
    font.setPixelSize(45);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(Qt::blue);

    QFontMetrics fontMetrics(font);
    QString temp_str = QString::number(temperature_, 'f', 1);
    int font_width = fontMetrics.width(temp_str);
    painter.drawText((arc_width - font_width) / 2, up_arc_height / 2 - 43,
                     temp_str);

    // draw test temperature point
    painter.setBrush(QBrush(Qt::red));
    painter.drawEllipse(QPoint(arc_width / 2, up_arc_height / 2 - 20), 15, 15);
  }

 private:
  float temperature_;
};

class OutlineWidget : public QWidget {
  Q_OBJECT

 public:
  OutlineWidget(int width, int height, QWidget *parent = nullptr);
  ~OutlineWidget() override;

 private slots:
  void rx_warn_distance();
  void rx_update();
  void rx_temperature(float temp);

 private:
  QTimer timer_;

  QString background_style_, no_style_;

  bool show_valid_rect_;
  TemperatureTipLabel *pl_temperature_;
};

}  // namespace suanzi

#endif
