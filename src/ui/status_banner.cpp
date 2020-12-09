#include "status_banner.hpp"

#include <QDateTime>
#include <QFontDatabase>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <QStringList>

#include "config.hpp"
#include "system.hpp"

using namespace suanzi;

StatusBanner::StatusBanner(int screen_width, int screen_height, QWidget *parent)
    : QWidget(parent), name_(""), last_name_("") {
  setAttribute(Qt::WA_StyledBackground, true);

  person_service_ = PersonService::get_instance();
  db_ = std::make_shared<FaceDatabase>(Config::get_quface().db_name);
  db_->size(db_size_);

  setStyleSheet(
      "QWidget {background-color:rgba(5, 0, 20, 150);margin:0px;} QLabel "
      "{background-color:transparent;}");
  setFixedSize(screen_width, 0.02734375 * screen_height);

  move(0, 0);

  const int w = screen_width;
  const int h = screen_height;

  // people icon
  QLabel *pl_person = new QLabel(this);
  pl_person->setFixedSize(0.025 * w, 0.0140625 * h);
  pl_person->setStyleSheet("QLabel {border-image: url(:asserts/people.png);}");

  pl_person_num_ = new QLabel(this);
  QString style_str =
      "QLabel {color: rgba(255, 255, 255, 200);font-weight:bold;font-size:";
  style_str += QString::number(0.02125 * w) + "pt;}";
  pl_person_num_->setStyleSheet(style_str);
  pl_person_num_->setAlignment(Qt::AlignTop | Qt::AlignRight);

  // temperature icon
  pl_temperature_ = new QLabel(this);
  pl_temperature_->setFixedSize(0.0375 * w, 0.0234375 * h);
  pl_temperature_->setStyleSheet(
      "QLabel {border-image: url(:asserts/temperature.png);}");

  // face icon
  QLabel *pl_face = new QLabel(this);
  pl_face->setFixedSize(0.0275 * w, 0.0171875 * h);
  pl_face->setStyleSheet("QLabel {border-image: url(:asserts/face.png);}");

  // network icon
  pl_net_ = new QLabel(this);
  pl_net_->setFixedSize(0.0275 * w, 0.0171875 * h);
  pl_net_->setStyleSheet(
      "QLabel {border-image: url(:asserts/no_network.png);}");

  QHBoxLayout *ph_layout = new QHBoxLayout;
  ph_layout->addWidget(pl_person, 0, Qt::AlignLeft | Qt::AlignCenter);
  ph_layout->addWidget(pl_person_num_, 0, Qt::AlignCenter);
  ph_layout->addStretch();

  ph_layout->addWidget(pl_temperature_, 0, Qt::AlignRight | Qt::AlignCenter);
  ph_layout->addWidget(pl_face, 0, Qt::AlignRight | Qt::AlignCenter);
  ph_layout->addSpacing(0.00625 * w);
  ph_layout->addWidget(pl_net_, 0, Qt::AlignRight | Qt::AlignCenter);
  ph_layout->setSpacing(0.00625 * w);
  ph_layout->setContentsMargins(0.0125 * w, 0.00234375 * h, 0.0125 * w,
                                0.00234375 * h);

  setLayout(ph_layout);

  rx_update();
  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(rx_update()));
  timer_->start(1000);
}

StatusBanner::~StatusBanner() { timer_->stop(); }

void StatusBanner::rx_update() {
  if (SZ_RETCODE_OK != db_->size(db_size_)) db_size_ = 0;
  pl_person_num_->setNum((int)db_size_);

  auto cfg = Config::get_user();
  if (cfg.enable_temperature) {
    pl_temperature_->show();
  } else {
    pl_temperature_->hide();
  }

  System::get_current_network(name_, ip_, mac_);
  if (last_name_ != name_) {
    last_name_ = name_;
    QString style_str = "QLabel {border-image: url(:asserts/no_network.png);}";
    if (last_name_ == "eth0") {
      style_str = "QLabel {border-image: url(:asserts/wired_network.png);}";
    } else if (last_name_ == "wlan0") {
      style_str = "QLabel {border-image: url(:asserts/wireless_network.png);}";
    }
    pl_net_->setStyleSheet(style_str);
  }
}

void StatusBanner::rx_display(bool invisible) {
  if (invisible)
    hide();
  else
    show();
}
