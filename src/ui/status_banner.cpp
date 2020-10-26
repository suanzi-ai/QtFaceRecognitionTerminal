#include "status_banner.hpp"

#include <QDateTime>
#include <QFontDatabase>
#include <QPainter>
#include <QStringList>

#include "config.hpp"
#include "system.hpp"

using namespace suanzi;

StatusBanner::StatusBanner(int width, int height, QWidget *parent)
    : icon_("loc.png"),
      icon2_("people.png"),
      icon3_("outline.png"),
      icon4_("temperature.png"),
      icon5_("earth.png"),
      icon6_("face.png"),
      icon7_("file.png"),
      QWidget(parent) {

  person_service_ = PersonService::get_instance();

  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);

  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(rx_update()));
  timer_->start(1000);

  int fontId = QFontDatabase::addApplicationFont("number.ttf");
  QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);

  font_.setFamily(fontFamilies.at(0));
}

StatusBanner::~StatusBanner() { timer_->stop(); }

void StatusBanner::rx_update() {
  std::string ip, name;
  System::get_current_network(name, ip);
  ip_address_ = name + " " + ip;

  System::get_release_version(version_);
}

void StatusBanner::paint(QPainter *painter) {
  const int w = width();
  const int h = height();

  auto cfg = Config::get_user();

  painter->fillRect(QRect(0, 0, w, 35), QColor(5, 0, 20, 150));
  painter->fillRect(QRect(0, h - 220, w, 220), QColor(5, 0, 20, 150));
  painter->setPen(QPen(QColor(150, 100, 0, 150), 2));
  painter->drawLine(0, h - 220, w, h - 220);
  // painter.drawLine(w * 40 / 100, 50, w * 40 / 100, h - 50);

  // draw datetime
  QDateTime now = QDateTime::currentDateTime();
  QString time = now.toString("hh mm");
  QString date = now.toString("yyyy年MM月dd日");

  // int base_font_size = h * 5 / 100;

  painter->setPen(QPen(QColor(255, 255, 255, 200), 2));
  QFont font = painter->font();
  font_.setPointSize(50);
  painter->setFont(font_);
  painter->drawText(30, h - 105, time);
  font_.setPointSize(40);
  painter->setFont(font_);
  painter->drawText(108, h - 105, ":");

  painter->drawLine(223, h - 100, 223, h - 150);

  font_.setPointSize(17);
  painter->setFont(font_);
  painter->drawText(235, h - 132, date);
  font_.setPointSize(16);
  painter->setFont(font_);
  painter->drawText(235, h - 102, "土曜日");

  font_.setPointSize(17);
  painter->setFont(font_);
  painter->drawText(45, h - 20, "SN:2020060229 FW:1.9.8");
  painter->setFont(font);

  font_.setPointSize(17);
  painter->setFont(font_);
  painter->drawText(33, 25, "1");

  painter->drawPixmap(QRect(43, h - 78, 20, 28), icon_, QRect());

  painter->drawPixmap(QRect(8, 10, 20, 18), icon2_, QRect());
  painter->drawPixmap(QRect(0, 240, 800, 790), icon3_, QRect());
  painter->drawPixmap(QRect(700, 3, 30, 30), icon4_, QRect());
  painter->drawPixmap(QRect(768, 7, 22, 22), icon5_, QRect());
  painter->drawPixmap(QRect(735, 7, 22, 22), icon6_, QRect());
  painter->drawPixmap(QRect(672, 5, 25, 23), icon7_, QRect());

  // font->setPixelSize(base_font_size * 2);
  // painter->setFont(font);
  // painter->drawText(w * 7 / 100, h - 150, date);

  // painter->setPen(Qt::green);
  // painter->drawText(20, 40, QString(ip_address_.c_str()));
  // painter->drawText(w - version_.length() * 9, 40, QString(version_.c_str()));
}
