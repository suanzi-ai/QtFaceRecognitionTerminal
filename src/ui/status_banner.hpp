#ifndef STATUS_BANNER_H
#define STATUS_BANNER_H

#include <QWidget>

namespace suanzi {

class StatusBanner : public QWidget {
  Q_OBJECT

 public:
  StatusBanner(int width, int height, QWidget *parent = nullptr);
  ~StatusBanner() override;

  void paint(QPainter *painter);

 private slots:
  void rx_ip(std::string ip_address);
  void rx_version(std::string version);

 private:
  std::string ip_address_;
  std::string version_;
};

}  // namespace suanzi

#endif
