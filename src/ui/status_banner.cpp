#include "status_banner.hpp"

#include <QPainter>

using namespace suanzi;

StatusBanner::StatusBanner(PersonService::ptr person_service, int width,
                           int height, QWidget *parent)
    : person_service_(person_service), QWidget(parent) {
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
  ip_address_ = person_service_->get_local_ip();
  version_ = person_service_->get_system_version();
}

void StatusBanner::paint(QPainter *painter) {
  const int w = width();
  const int h = height();

  painter->setPen(Qt::green);
  painter->drawText(20, 40, QString(ip_address_.c_str()));
  painter->drawText(w - version_.length() * 9, 40, QString(version_.c_str()));
}
