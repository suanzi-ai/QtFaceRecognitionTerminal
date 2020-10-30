#include "recognize_tip_widget.hpp"

#include <QDateTime>
#include <QFontDatabase>
#include <QImage>
#include <QLocale>
#include <QPainter>
#include <QStringList>
#include <QTimer>

#include <opencv2/opencv.hpp>

#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

RecognizeTipWidget::RecognizeTipWidget(int width, int height, QWidget *parent)
    : QWidget(parent),
      icon_(":asserts/location.png"),
      icon_good_(":asserts/tick.png"),
      icon_bad_(":asserts/cross.png"),
      has_info_(false) {
  QPalette palette = this->palette();
  palette.setColor(QPalette::Background, Qt::transparent);
  setPalette(palette);

  move(0, 0);
  setFixedSize(width, height);

  font_.setFamily(QFontDatabase::applicationFontFamilies(
                      QFontDatabase::addApplicationFont(":asserts/clock.ttf"))
                      .at(0));

  int w = width, h = height;
  temperature_rect_.addRoundedRect(
      QRect(0.25625 * w, 0.109375 * h, 0.4875 * w, 0.05078125 * h), 0.04375 * w,
      0.02734375 * h);

  timer_.setInterval(2000);
  timer_.setSingleShot(true);
  connect((const QObject *)&timer_, SIGNAL(timeout()), (const QObject *)this,
          SLOT(rx_reset()));
}

RecognizeTipWidget::~RecognizeTipWidget() {}

void RecognizeTipWidget::rx_display(PersonData person, bool if_duplicated) {
  avatar_.load(person_.face_path.c_str());

  if ((!has_info_ || person_.face_path != person.face_path) &&
      !person.face_snapshot.empty()) {
    cv::cvtColor(person.face_snapshot, person.face_snapshot, CV_BGR2RGB);
    snapshot_ = QPixmap::fromImage(QImage(
        (unsigned char *)person.face_snapshot.data, person.face_snapshot.cols,
        person.face_snapshot.rows, QImage::Format_RGB888));

    last_person_ = person.face_path;
  }

  timer_.stop();
  timer_.start();

  has_info_ = true;
  person_ = person;
}

void RecognizeTipWidget::rx_reset() { has_info_ = false; }

void RecognizeTipWidget::paint(QPainter *painter) {
  const int w = width();
  const int h = height();

  auto lang = Config::get_user_lang();

  // draw background
  painter->fillRect(QRect(0, 0.84375 * h, w, 0.171875 * h),
                    QColor(5, 0, 20, 150));

  // draw border and seperator
  painter->setPen(QPen(QColor(150, 100, 0, 150), 2));
  painter->drawLine(0, 0, w, 0);
  painter->setPen(QPen(QColor(255, 255, 255, 200), 2));
  painter->drawLine(0.27875 * w, 0.8828125 * h, 0.27875 * w, 0.921875 * h);

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
  QString time = now.toString("hh mm");
  QString date = now.toString(tr("yyyy年MM月dd日"));
  QString weekday = locale.toString(now, format);

  QFont font = painter->font();
  font_.setPointSize(0.0625 * w);
  painter->setFont(font_);
  painter->drawText(0.0375 * w, 0.9203125 * h, time);
  font_.setPointSize(0.05 * w);
  painter->setFont(font_);
  painter->drawText(0.135 * w, 0.9203125 * h, ":");

  font_.setPointSize(0.02125 * w);
  painter->setFont(font_);
  painter->drawText(0.29375 * w, 0.896875 * h, date);
  font_.setPointSize(0.02 * w);
  painter->setFont(font_);
  painter->drawText(0.29375 * w, 0.9203125 * h, weekday);

  // draw SN and version
  font_.setPointSize(0.02125 * w);
  painter->setFont(font_);
  painter->drawText(0.05625 * w, 0.984375 * h, "SN:2020060229 FW:1.9.8");

  // draw icon
  painter->drawPixmap(
      QRect(0.05375 * w, 0.9390625 * h, 0.025 * w, 0.021875 * h), icon_,
      QRect());

  // draw avatar here
  if (has_info_) {
    auto cfg = Config::get_user();
    painter->drawPixmap(
        QRect(0.8125 * w, 0.8828125 * h, 0.1375 * w, 0.0859375 * h), snapshot_,
        QRect());
    if (person_.is_status_normal())
      painter->drawPixmap(
          QRect(0.625 * w, 0.8828125 * h, 0.1375 * w, 0.0859375 * h), avatar_,
          QRect());

    if (Config::get_user().enable_temperature) {
      char temperature_value[10];
      sprintf(temperature_value, ":%.1f°C", person_.temperature);

      painter->setRenderHint(QPainter::Antialiasing);

      auto cfg = Config::get_user();

      QColor color;
      if (person_.is_temperature_normal()) {
        color = QColor(0, 100, 0, 180);
        painter->drawPixmap(
            QRect(0.275 * w, 0.11796875 * h, 0.05625 * w, 0.03515625 * h),
            icon_good_, QRect());
      } else {
        color = QColor(cfg.red, cfg.green, cfg.blue, cfg.alpha);
        painter->drawPixmap(
            QRect(0.275 * w, 0.11796875 * h, 0.05625 * w, 0.03515625 * h),
            icon_bad_, QRect());
      }
      painter->fillPath(temperature_rect_, color);

      font_.setPointSize(0.02875 * w);
      painter->setFont(font_);
      painter->drawText(0.35 * w, 0.14453125 * h,
                        tr("体温") + temperature_value);
    }
  }

  painter->setFont(font);
}
