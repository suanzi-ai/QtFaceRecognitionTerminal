#include "recognize_tip_widget.hpp"

#include <sstream>

#include <QDateTime>
#include <QFontDatabase>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLocale>
#include <QPainter>
#include <QStringList>
#include <QThread>
#include <QTimer>
#include <QVBoxLayout>

#include <opencv2/opencv.hpp>

#include "config.hpp"
#include "system.hpp"

using namespace suanzi;
using namespace suanzi::io;

RecognizeTipWidget::RecognizeTipWidget(int screen_width, int screen_height,
                                       QWidget *parent)
    : QWidget(parent), has_info_(false) {
  setAttribute(Qt::WA_StyledBackground, true);

  style_ =
      "QWidget { background-color:%1; margin:0px; } "
      "QLabel { background-color:transparent; color:white; "
      "border: 1px solid transparent; padding: 0px 0px 0px 0px; }";
  setStyleSheet(style_.arg("rgba(5, 0, 20, 150)"));

  const int w = screen_width;
  const int h = screen_height;

  move(0, 0.84375 * h);
  setFixedSize(w, 0.171875 * h);

  font_.setFamily(QFontDatabase::applicationFontFamilies(
                      QFontDatabase::addApplicationFont(":asserts/clock.ttf"))
                      .at(0));

  auto cfg = Config::get_user();
  // draw time
  font_.setPointSize(0.0625 * w);
  pl_hh_mm_ = new QLabel(this);
  pl_hh_mm_->setFont(font_);
  pl_hh_mm_->setFixedHeight(0.055 * h);
  pl_hh_mm_->move(0.0375 * w, 0.03 * h);
  pl_hh_mm_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pl_hh_mm_->setIndent(0);

  // draw seperator
  QLabel *pl_separator = new QLabel(this);
  pl_separator->setFixedSize(0.0025 * w, 0.04 * h);
  pl_separator->setStyleSheet("QLabel {background-color:white;}");
  pl_separator->move(0.28125 * w, 0.04 * h);

  // draw date and weekday
  font_.setPointSize(0.02125 * w);
  pl_yyyy_mm_dd_ = new QLabel(this);
  pl_yyyy_mm_dd_->setFont(font_);
  pl_yyyy_mm_dd_->setFixedHeight(0.0234375 * h);
  pl_yyyy_mm_dd_->move(0.3 * w, 0.035 * h);
  pl_yyyy_mm_dd_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pl_yyyy_mm_dd_->setIndent(0);

  pl_week_ = new QLabel(this);
  pl_week_->setFont(font_);
  pl_week_->setFixedHeight(0.0234375 * h);
  pl_week_->move(0.3 * w, 0.0586 * h);
  pl_week_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pl_week_->setIndent(0);

  // draw location icon and device name
  QLabel *pl_location_icon = new QLabel(this);
  pl_location_icon->setFixedSize(0.03125 * w, 0.025 * h);
  pl_location_icon->setStyleSheet(
      "QLabel {border-image: url(:asserts/location.png);}");
  pl_location_icon->move(0.05625 * w, 0.09 * h);

  // draw SN, FW and ip
  pl_host_name_ = new QLabel(this);
  pl_host_name_->setFont(font_);
  pl_host_name_->setFixedHeight(0.0234375 * h);
  pl_host_name_->move(0.1 * w, 0.09 * h);
  pl_host_name_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pl_host_name_->setIndent(0);

  pl_sn_ = new QLabel(this);
  pl_sn_->setFont(font_);
  pl_sn_->setFixedHeight(0.0234375 * h);
  pl_sn_->move(0.05625 * w, 0.1171875 * h);
  pl_sn_->setAlignment(Qt::AlignTop | Qt::AlignLeft);
  pl_sn_->setIndent(0);

  // draw avatar and snapshot
  pl_avatar_ = new QLabel(this);
  pl_avatar_->setScaledContents(true);
  pl_avatar_->setFixedSize(0.1375 * w, 0.0859375 * h);
  pl_avatar_->move(0.625 * w, 0.04 * h);

  pl_snapshot_ = new QLabel(this);
  pl_snapshot_->setScaledContents(true);
  pl_snapshot_->setFixedSize(0.1375 * w, 0.0859375 * h);
  pl_snapshot_->move(0.8125 * w, 0.04 * h);

  reset_timer_.setInterval(2000);
  reset_timer_.setSingleShot(true);
  connect((const QObject *)&reset_timer_, SIGNAL(timeout()),
          (const QObject *)this, SLOT(rx_reset()));

  rx_update();
  connect((const QObject *)&ip_timer_, SIGNAL(timeout()), (const QObject *)this,
          SLOT(rx_update()));
  ip_timer_.start(1000);
}

RecognizeTipWidget::~RecognizeTipWidget() {}

void RecognizeTipWidget::rx_display(PersonData person, bool audio_duplicated,
                                    bool record_duplicated) {
  person_ = person;

  // avatar_.load(person_.face_path.c_str());
  cv::Mat avatar = cv::imread(person_.face_path.c_str());
  if (!avatar.empty()) {
    cv::cvtColor(avatar, avatar, CV_BGR2RGB);
    avatar_ = QPixmap::fromImage(QImage((unsigned char *)avatar.data,
                                        avatar.cols, avatar.rows, avatar.step,
                                        QImage::Format_RGB888));
    avatar.release();
  }

  if (!record_duplicated && !person.face_snapshot.empty()) {
    cv::cvtColor(person.face_snapshot, person.face_snapshot, CV_BGR2RGB);
    snapshot_ = QPixmap::fromImage(
        QImage((unsigned char *)person.face_snapshot.data,
               person.face_snapshot.cols, person.face_snapshot.rows,
               person.face_snapshot.step, QImage::Format_RGB888));
  }

  bool btemperature = false;
  bool bnormal_temperature = false;
  check_temperature(btemperature, bnormal_temperature);
  emit tx_temperature(btemperature, bnormal_temperature, person_.temperature);

  if (btemperature && !bnormal_temperature)
    setStyleSheet(style_.arg("rgba(220, 0, 0, 150)"));
  else
    setStyleSheet(style_.arg("rgba(5, 0, 20, 150)"));

  reset_timer_.stop();
  reset_timer_.start();

  if (person_.is_status_normal()) {
    pl_avatar_->setPixmap(avatar_);
    pl_avatar_->show();
  }
  if (!person.face_snapshot.empty()) {
    pl_snapshot_->setPixmap(snapshot_);
    pl_snapshot_->show();
  }
}

void RecognizeTipWidget::rx_update() {
  System::get_current_network(name_, ip_, mac_);
  System::get_hostname(hostname_);
  System::get_serial_number(serial_number_);
  System::get_fw_version(fw_version_);

  if (name_ == "eth0" || name_ == "wlan0") {
    std::istringstream sin(ip_);
    std::string seg;
    std::ostringstream sout;
    while (std::getline(sin, seg, '.')) {
      int value = atoi(seg.c_str());
      sout << std::setfill('0') << std::setw(2) << std::hex << value;
    }
    ip_ = sout.str();
  }
  std::transform(ip_.begin(), ip_.end(), ip_.begin(), ::toupper);
  std::transform(mac_.begin(), mac_.end(), mac_.begin(), ::toupper);

  pl_host_name_->setText(hostname_.c_str());
  pl_host_name_->adjustSize();
  QString sn_str =
      QString("SN:%1 FW:%2%3")
          .arg(serial_number_.c_str(), fw_version_.c_str(), ip_.c_str());

  // 去掉换行符
  sn_str = sn_str.replace(QRegExp("\\\n"), "");
  pl_sn_->setText(sn_str);
  pl_sn_->adjustSize();
  rx_timeout();
}

void RecognizeTipWidget::rx_reset() {
  setStyleSheet(style_.arg("rgba(5, 0, 20, 150)"));
  emit tx_temperature(false, false, 0);

  pl_avatar_->hide();
  pl_snapshot_->hide();
}

void RecognizeTipWidget::check_temperature(bool &btemperature,
                                           bool &bnormal_temperature) {
  btemperature = false;
  bnormal_temperature = true;
  if (Config::get_user().enable_temperature && person_.temperature > 0) {
    btemperature = true;
    bnormal_temperature = person_.is_temperature_normal();
  }
}

void RecognizeTipWidget::rx_timeout() {
  auto lang = Config::get_user_lang();
  auto cfg = Config::get_user();

  // draw datetime
  QLocale locale;
  QString format = "ddd";
  if (lang == "zh-CN") locale = QLocale::Chinese;
  if (lang == "en") locale = QLocale::English;
  if (lang == "jp") {
    locale = QLocale::Japanese;
    format = "ddd曜日";
  }
  QDateTime now = QDateTime::currentDateTime();
  QString time = now.toString("hh:mm");
  QString date = now.toString(tr("yyyy年MM月dd日"));
  QString weekday = locale.toString(now, format);

  pl_hh_mm_->setText(time);
  pl_yyyy_mm_dd_->setText(date);
  pl_week_->setText(weekday);
}
