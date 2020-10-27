#include "status_banner.hpp"

#include <QPainter>

#include "system.hpp"

using namespace suanzi;

StatusBanner::StatusBanner(int width, int height, QWidget *parent)
    : QWidget(parent) {

  person_service_ = PersonService::get_instance();

  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);

  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(rx_update()));
  timer_->start(1000);
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

  painter->setPen(Qt::green);
  painter->drawText(20, 40, QString(ip_address_.c_str()));
  painter->drawText(w - version_.length() * 9, 40, QString(version_.c_str()));
}
