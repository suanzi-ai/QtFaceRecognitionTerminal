#include "status_banner.hpp"

#include <QPainter>

using namespace suanzi;

StatusBanner::StatusBanner(int width, int height, QWidget *parent)
    : QWidget(parent) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);

  ip_address_ = "eth0: 192.168.xxx.xxx";
  version_ = "version 1.0.0";
}

StatusBanner::~StatusBanner() {}

void StatusBanner::rx_ip(std::string ip_address) {
  ip_address_ = ip_address;
}

void StatusBanner::rx_version(std::string version) {
  version_ = version;
}

void StatusBanner::paint(QPainter *painter) {
  const int w = width();
  const int h = height();

  painter->drawText(20, 40, QString(ip_address_.c_str()));
  painter->drawText(w - 230, 40, QString(version_.c_str()));  
}
