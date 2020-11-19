#include "screen_saver_widget.hpp"

#include <QDateTime>
#include <QFile>
#include <QPaintEvent>
#include <QPainter>
#include <quface/logger.hpp>

#include "config.hpp"

using namespace suanzi;

ScreenSaverWidget::ScreenSaverWidget(int width, int height, QWidget *parent)
    : QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);

  std::vector<SZ_BYTE> img;
  if (Config::read_screen_saver_background(img)) {
    background_.loadFromData(img.data(), img.size());
  }

  background_.scaled(width, height);
}

ScreenSaverWidget::~ScreenSaverWidget() {}

void ScreenSaverWidget::rx_display(int disappear_seconds) {
  auto cfg = Config::get_user();
  if (cfg.enable_screensaver && disappear_seconds > cfg.screensaver_timeout) {
    hide();
    show();
  }
}

void ScreenSaverWidget::rx_hide() { hide(); }

void ScreenSaverWidget::paintEvent(QPaintEvent *event) {
  if (!Config::get_user().enable_screensaver)
    hide();
  else {
    QPainter painter(this);
    const int w = width();
    const int h = height();

    // draw background
    QPixmap pixmapToShow = QPixmap::fromImage(background_);
    painter.drawPixmap(QRect(0, 0, w, h), pixmapToShow, QRect());

    // draw datetime
    QDateTime now = QDateTime::currentDateTime();

    QString time = now.toString("hh   mm");
    QString date = now.toString(tr("yyyy年MM月dd日"));

    int base_font_size = h * 2 / 100;

    QFont font = painter.font();
    font.setPixelSize(base_font_size * 5);
    painter.setFont(font);
    painter.setPen(Qt::white);
    painter.drawText(w * 25 / 100, h * 33 / 100, time);

    bool flag = (now.currentMSecsSinceEpoch() / 500) % 2;
    if (flag) painter.drawText(w * 47 / 100, h * 33 / 100, ":");

    font.setPixelSize(base_font_size * 2);
    painter.setFont(font);
    painter.drawText(w * 26 / 100, h * 40 / 100, date);
  }
}
