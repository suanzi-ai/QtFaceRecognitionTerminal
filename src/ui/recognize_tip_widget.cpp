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
    : QWidget(parent),
      icon_(":asserts/location.png"),
      icon_good_(":asserts/tick.png"),
      icon_bad_(":asserts/cross.png"),
      has_info_(false) {
  setAttribute(Qt::WA_StyledBackground, true);
  setStyleSheet(
      "QWidget {background-color:rgba(5, 0, 20, 150);margin:0px;} QLabel "
      "{background-color:transparent;color:white;border: 1px solid "
      "transparent;padding: 0px 0px 0px 0px;}");
  move(0, 0.84375 * screen_height);
  setFixedSize(screen_width, 0.171875 * screen_height);

  //小时 分钟 布局
  pl_hh_mm_ = new QLabel(this);
  pl_hh_mm_->setAlignment(Qt::AlignTop);

  QFont font;
  font.setPointSize(0.0625 * screen_width);
  pl_hh_mm_->setFont(font);
  pl_hh_mm_->setText("00:00");
  QSize tmp_size = pl_hh_mm_->sizeHint();
  int h = tmp_size.height() - 40;
  pl_hh_mm_->setFixedHeight(h);
  pl_hh_mm_->setIndent(2);

  QHBoxLayout *ph_layout = new QHBoxLayout;
  ph_layout->addWidget(pl_hh_mm_);
  ph_layout->addSpacing(10);

  //分割线
  QLabel *pl_separator = new QLabel(this);
  pl_separator->setFixedSize(2, h - 15);
  pl_separator->setStyleSheet("QLabel {background-color:white;}");
  ph_layout->addWidget(pl_separator);

  //年月日 周 布局
  font.setPointSize(0.02125 * screen_width);
  pl_yyyy_mm_dd_ = new QLabel(this);
  pl_yyyy_mm_dd_->setFont(font);
  pl_week_ = new QLabel(this);
  pl_week_->setFont(font);
  QVBoxLayout *pv_layout = new QVBoxLayout;
  pv_layout->addSpacing(5);
  pv_layout->addWidget(pl_yyyy_mm_dd_);
  pv_layout->addWidget(pl_week_);
  pv_layout->setSpacing(0);
  pv_layout->setMargin(0);
  pv_layout->addStretch();

  ph_layout->addLayout(pv_layout);
  ph_layout->setSpacing(0);
  ph_layout->setMargin(0);

  QGridLayout *pg_layout = new QGridLayout;
  pg_layout->addLayout(ph_layout, 1, 0, 2, 6, Qt::AlignLeft | Qt::AlignBottom);

  //位置 主机名 布局
  QLabel *pl_location_icon = new QLabel(this);
  pl_location_icon->setFixedSize(0.025 * screen_width,
                                 0.021875 * screen_height);
  pl_location_icon->setStyleSheet(
      "QLabel {border-image: url(:asserts/location.png);}");
  pl_host_name_ = new QLabel(this);
  pl_host_name_->setText("quface");
  pl_host_name_->setFixedHeight(0.021875 * screen_height);

  ph_layout = new QHBoxLayout;
  ph_layout->addSpacing(20);
  ph_layout->addWidget(pl_location_icon);
  ph_layout->addWidget(pl_host_name_);
  ph_layout->setMargin(0);
  ph_layout->setSpacing(5);
  pg_layout->addLayout(ph_layout, 3, 0, 1, 2, Qt::AlignLeft | Qt::AlignTop);

  pl_sn_ = new QLabel(this);
  font.setPointSize(0.02125 * screen_width);
  pl_sn_->setFont(font);
  pl_sn_->setIndent(20);
  pl_sn_->setTextFormat(Qt::PlainText);
  pg_layout->addWidget(pl_sn_, 4, 0, 1, 6, Qt::AlignLeft | Qt::AlignTop);

  // 抓拍图片
  pl_avatar_ = new QLabel(this);
  pl_avatar_->setScaledContents(true);
  pl_avatar_->setFixedSize(0.1375 * screen_width, 0.0859375 * screen_height);
  pl_snapshot_ = new QLabel(this);
  pl_snapshot_->setScaledContents(true);
  pl_snapshot_->setFixedSize(0.1375 * screen_width, 0.0859375 * screen_height);

  pg_layout->addWidget(pl_avatar_, 0, 6, 6, 3, Qt::AlignLeft | Qt::AlignCenter);
  pg_layout->addWidget(pl_snapshot_, 0, 9, 6, 3,
                       Qt::AlignLeft | Qt::AlignCenter);

  setLayout(pg_layout);

  /*font_.setFamily(QFontDatabase::applicationFontFamilies(
                      QFontDatabase::addApplicationFont(":asserts/clock.ttf"))
                      .at(0));

  int w = width, h = height;
  temperature_rect_.addRoundedRect(
      QRect(0.25625 * w, 0.109375 * h, 0.4875 * w, 0.05078125 * h), 0.04375 * w,
      0.02734375 * h);*/

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

  if (bnormal_temperature)
    setStyleSheet(
        "QWidget {background-color:rgba(5, 0, 20, 150);margin:0px;} QLabel "
        "{background-color:transparent;color:white;border: 1px solid "
        "transparent;padding: 0px 0px 0px 0px;}");
  else
    setStyleSheet(
        "QWidget {background-color:rgba(220, 0, 0, 150);margin:0px;} QLabel "
        "{background-color:transparent;color:white;border: 1px solid "
        "transparent;padding: 0px 0px 0px 0px;}");

  reset_timer_.stop();
  reset_timer_.start();

  if (person_.is_status_normal()) {
    pl_avatar_->setPixmap(avatar_);
    pl_avatar_->show();
  }
  pl_snapshot_->setPixmap(snapshot_);
  pl_snapshot_->show();
}

void RecognizeTipWidget::rx_update() {
  System::get_current_network(name_, ip_, mac_);
  System::get_hostname(hostname_);
  System::get_serial_number(serial_number_);

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
  QString sn_str = "SN:";
  sn_str += serial_number_.c_str();
  sn_str += " FW:1.1.1";
  sn_str += ip_.c_str();

  //去掉换行符
  sn_str = sn_str.replace(QRegExp("\\\n"), "");
  pl_sn_->setText(sn_str);

  rx_timeout();
}

void RecognizeTipWidget::rx_reset() {
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
