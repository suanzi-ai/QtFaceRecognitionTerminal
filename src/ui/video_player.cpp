#include "video_player.hpp"

#include <QBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QTimer>

#include "config.hpp"
#include "dashu_task.hpp"
#include "temperature_task.hpp"

using namespace suanzi;

VideoPlayer::VideoPlayer(FaceDatabasePtr db, FaceDetectorPtr detector,
                         FacePoseEstimatorPtr pose_estimator,
                         FaceExtractorPtr extractor,
                         FaceAntiSpoofingPtr anti_spoofing,
                         PersonService::ptr person_service, QWidget *parent)
    : person_service_(person_service), QWidget(parent) {
  // Initialize QT widget
  QPalette pal = palette();
  pal.setColor(QPalette::Background, Qt::transparent);
  pal.setColor(QPalette::Foreground, Qt::green);
  setPalette(pal);

  auto app = Config::get_app();
  auto quface = Config::get_quface();

  camera_reader_ = new CameraReader(this);

  int screen_width, screen_height;
  if (!camera_reader_->get_screen_size(screen_width, screen_height)) {
    SZ_LOG_ERROR("Get screen size error");
  } else {
    SZ_LOG_INFO("Get screen w={}, h={}", screen_width, screen_height);
  }

  isp_hist_widget_ = new ISPHistWidget(this);

  detect_tip_widget_bgr_ =
      new DetectTipWidget(0, 0, screen_width, screen_height, this);
  detect_tip_widget_bgr_->hide();

  detect_tip_widget_nir_ = new DetectTipWidget(
      screen_width - screen_width * 25 / 100, 0, screen_width * 25 / 100,
      screen_height * 25 / 100, this);
  detect_tip_widget_nir_->hide();

  recognize_tip_widget_ = new RecognizeTipWidget(nullptr);
  recognize_tip_widget_->hide();

  screen_saver_ = new ScreenSaverWidget(screen_width, screen_height, nullptr);
  screen_saver_->hide();

  // Initialize QThreads
  camera_reader_ = new CameraReader(this);

  detect_task_ = new DetectTask(detector, pose_estimator, nullptr, this);
  recognize_task_ =
      new RecognizeTask(db, extractor, anti_spoofing, nullptr, this);
  record_task_ = new RecordTask(person_service, nullptr, this);
  face_timer_ = new FaceTimer(nullptr, this);

  // IO Tasks
  upload_task_ = new UploadTask(person_service, nullptr, this);
  audio_task_ = new AudioTask(nullptr, this);

  // Connect camera_reader to detect_task
  connect((const QObject *)camera_reader_,
          SIGNAL(tx_frame(PingPangBuffer<ImagePackage> *)),
          (const QObject *)detect_task_,
          SLOT(rx_frame(PingPangBuffer<ImagePackage> *)));
  connect((const QObject *)detect_task_, SIGNAL(tx_finish()),
          (const QObject *)camera_reader_, SLOT(rx_finish()));

  // Connect detect_task to recognize_task
  connect((const QObject *)detect_task_,
          SIGNAL(tx_frame(PingPangBuffer<DetectionData> *)),
          (const QObject *)recognize_task_,
          SLOT(rx_frame(PingPangBuffer<DetectionData> *)));
  connect((const QObject *)recognize_task_, SIGNAL(tx_finish()),
          (const QObject *)detect_task_, SLOT(rx_finish()));

  // Connect recognize_task to record_task
  connect((const QObject *)recognize_task_,
          SIGNAL(tx_frame(PingPangBuffer<RecognizeData> *)),
          (const QObject *)record_task_,
          SLOT(rx_frame(PingPangBuffer<RecognizeData> *)));
  connect((const QObject *)record_task_, SIGNAL(tx_finish()),
          (const QObject *)recognize_task_, SLOT(rx_finish()));
  connect((const QObject *)record_task_, SIGNAL(tx_nir_finish(bool)),
          (const QObject *)recognize_task_, SLOT(rx_nir_finish(bool)));
  connect((const QObject *)record_task_, SIGNAL(tx_bgr_finish(bool)),
          (const QObject *)recognize_task_, SLOT(rx_bgr_finish(bool)));

  // Connect detect_task to face_timer
  connect((const QObject *)detect_task_,
          SIGNAL(tx_frame(PingPangBuffer<DetectionData> *)),
          (const QObject *)face_timer_,
          SLOT(rx_frame(PingPangBuffer<DetectionData> *)));

  //   // Connect face_timer to record_task
  //   connect((const QObject *)face_timer_, SIGNAL(tx_face_disappear(int)),
  //           (const QObject *)record_task_, SLOT(rx_reset()));

  // Connect detect_task to detect_tip_widget
  connect((const QObject *)detect_task_,
          SIGNAL(tx_bgr_display(DetectionRatio, bool, bool)),
          (const QObject *)detect_tip_widget_bgr_,
          SLOT(rx_display(DetectionRatio, bool, bool)));
  if (Config::get_app().show_infrared_window) {
    connect((const QObject *)detect_task_,
            SIGNAL(tx_nir_display(DetectionRatio, bool, bool)),
            (const QObject *)detect_tip_widget_nir_,
            SLOT(rx_display(DetectionRatio, bool, bool)));
  }

  // Connect record_task to recognize_tip_widget
  connect((const QObject *)record_task_, SIGNAL(tx_display(PersonData, bool)),
          (const QObject *)recognize_tip_widget_,
          SLOT(rx_display(PersonData, bool)));

  // Connect record_task to upload_task_
  connect((const QObject *)record_task_, SIGNAL(tx_display(PersonData, bool)),
          (const QObject *)upload_task_, SLOT(rx_upload(PersonData, bool)));

  connect((const QObject *)record_task_, SIGNAL(tx_display(PersonData, bool)),
          (const QObject *)audio_task_, SLOT(rx_display(PersonData, bool)));

  // Connect face_timer to screen_saver
  connect((const QObject *)face_timer_, SIGNAL(tx_face_disappear(int)),
          (const QObject *)screen_saver_, SLOT(rx_display(int)));
  connect((const QObject *)face_timer_, SIGNAL(tx_face_appear(int)),
          (const QObject *)screen_saver_, SLOT(rx_hide()));

  // Connect temperature to record_task
  if (!Config::get_app().disabled_temperature) {
    temperature_task_ = new DashuTask();
    connect((const QObject *)temperature_task_, SIGNAL(tx_temperature(float)),
            (const QObject *)record_task_, SLOT(rx_temperature(float)));
  }

  camera_reader_->start_sample();

  QTimer::singleShot(1, this, SLOT(init_widgets()));

  static QTimer updateIpTimer;
  connect(&updateIpTimer, SIGNAL(timeout()), this, SLOT(update_ip_and_version()));
  updateIpTimer.start(1000);

  /*
  static Otpa16TempTask otpa16_task;
  connect((const QObject *)&otpa16_task, SIGNAL(tx_display(OtpaTempData)),
             (const QObject *)heat_map_widget_,
  SLOT(rx_display(OtpaTempData)));*/
}

VideoPlayer::~VideoPlayer() {}

void VideoPlayer::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  auto app = Config::get_app();
  int screen_width, screen_height;
  camera_reader_->get_screen_size(screen_width, screen_height);
  
  painter.drawText(20, 40, QString(ip_.c_str()));
  painter.drawText(screen_width - 230, 40, QString(version_.c_str()));
  
  if (!app.disabled_temperature) {
    int device_body_start_angle,
        device_body_open_angle;
    float x, y, face_width, face_height;
    x = (float)(app.device_face_x * screen_width);
    y = (float)(app.device_face_y * screen_height);
    face_width = (float)(app.device_face_width * screen_width);
    face_height = (float)(app.device_face_height * screen_height);
    device_body_start_angle = app.device_body_start_angle;
    device_body_open_angle = app.device_body_open_angle;
    painter.setPen(QPen(Qt::white, 5, Qt::SolidLine));
    painter.drawEllipse(x, y, face_width, face_height);
    painter.drawChord(x / 1.2, y + face_height + 25, face_width * 1.2, face_height, // +40是身体与头部的间距 *1.2是身体要比头宽一点
                      device_body_start_angle, device_body_open_angle);
    //     painter.drawRect(x, y, face_width, face_height);
  }

  detect_tip_widget_bgr_->paint(&painter);
  detect_tip_widget_nir_->paint(&painter);

  if (Config::get_app().show_isp_hist_window) {
    isp_hist_widget_->paint(&painter);
  }
}

void VideoPlayer::init_widgets() {
  auto app = Config::get_app();

  int screen_width, screen_height;
  camera_reader_->get_screen_size(screen_width, screen_height);

  int x = 0;
  int y = screen_height * app.recognize_tip_top_percent / 100;
  int w = screen_width;
  int h = screen_height - y;

  recognize_tip_widget_->setFixedSize(w, h);
  recognize_tip_widget_->move(x, y);

  isp_hist_widget_->setFixedSize(200, 160);
  isp_hist_widget_->move(0, 0);
}


void VideoPlayer::update_ip_and_version() {
	ip_ = person_service_->get_local_ip();
	version_ = person_service_->get_system_version();
}