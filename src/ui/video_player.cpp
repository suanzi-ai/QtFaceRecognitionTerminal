#include "video_player.hpp"

#include <QBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QTimer>

#include "config.hpp"

using namespace suanzi;

VideoPlayer::VideoPlayer(QWidget *parent) : QWidget(parent) {
  // 初始化工作流
  init_workflow();

  // 初始化QT控件
  init_widgets();

  // 启动摄像头读取数据
  camera_reader_->start_sample();
}

VideoPlayer::~VideoPlayer() {}

void VideoPlayer::paintEvent(QPaintEvent *event) {
  QPainter painter(this);

  detect_tip_widget_bgr_->paint(&painter);
  detect_tip_widget_nir_->paint(&painter);

  outline_widget_->paint(&painter);
  status_banner_->paint(&painter);
}

void VideoPlayer::init_workflow() {
  // 创建摄像头读取对象
  camera_reader_ = CameraReader::get_instance();

  // 创建人脸检测线程
  detect_task_ = DetectTask::get_instance();
  connect((const QObject *)camera_reader_,
          SIGNAL(tx_frame(PingPangBuffer<ImagePackage> *)),
          (const QObject *)detect_task_,
          SLOT(rx_frame(PingPangBuffer<ImagePackage> *)));
  connect((const QObject *)detect_task_, SIGNAL(tx_finish()),
          (const QObject *)camera_reader_, SLOT(rx_finish()));

  // 创建人脸识别线程
  recognize_task_ = RecognizeTask::get_instance();
  connect((const QObject *)detect_task_,
          SIGNAL(tx_frame(PingPangBuffer<DetectionData> *)),
          (const QObject *)recognize_task_,
          SLOT(rx_frame(PingPangBuffer<DetectionData> *)));

  // 创建人脸查询线程
  record_task_ = RecordTask::get_instance();
  connect((const QObject *)recognize_task_,
          SIGNAL(tx_frame(PingPangBuffer<RecognizeData> *)),
          (const QObject *)record_task_,
          SLOT(rx_frame(PingPangBuffer<RecognizeData> *)));
  connect((const QObject *)record_task_, SIGNAL(tx_nir_finish(bool)),
          (const QObject *)recognize_task_, SLOT(rx_nir_finish(bool)));
  connect((const QObject *)record_task_, SIGNAL(tx_bgr_finish(bool)),
          (const QObject *)recognize_task_, SLOT(rx_bgr_finish(bool)));

  // 创建继电器开关线程
  gpio_task_ = GPIOTask::get_instance();
  connect((const QObject *)record_task_, SIGNAL(tx_display(PersonData, bool)),
          (const QObject *)upload_task_, SLOT(rx_trigger(PersonData, bool)));

  // 创建人脸记录线程
  upload_task_ = UploadTask::get_instance();
  connect((const QObject *)record_task_, SIGNAL(tx_display(PersonData, bool)),
          (const QObject *)upload_task_, SLOT(rx_upload(PersonData, bool)));

  // 创建人脸计时器线程
  face_timer_ = FaceTimer::get_instance();
  connect((const QObject *)detect_task_,
          SIGNAL(tx_frame(PingPangBuffer<DetectionData> *)),
          (const QObject *)face_timer_,
          SLOT(rx_frame(PingPangBuffer<DetectionData> *)));

  // 创建语音播报线程
  audio_task_ = AudioTask::get_instance();
  connect((const QObject *)record_task_, SIGNAL(tx_report_person(PersonData)),
          (const QObject *)audio_task_, SLOT(rx_report_person(PersonData)));
  connect((const QObject *)record_task_, SIGNAL(tx_warn_mask()),
          (const QObject *)audio_task_, SLOT(rx_warn_mask()));
  connect((const QObject *)detect_task_, SIGNAL(tx_warn_distance()),
          (const QObject *)audio_task_, SLOT(rx_warn_distance()));

  // 创建人体测温线程
  temperature_task_ = TemperatureTask::get_instance();
  connect((const QObject *)temperature_task_, SIGNAL(tx_temperature(float)),
          (const QObject *)record_task_, SLOT(rx_temperature(float)));
  connect((const QObject *)face_timer_, SIGNAL(tx_face_disappear(int)),
          (const QObject *)temperature_task_, SLOT(rx_disable()));
  connect((const QObject *)face_timer_, SIGNAL(tx_face_appear(int)),
          (const QObject *)temperature_task_, SLOT(rx_enable()));
}

void VideoPlayer::init_widgets() {
  auto app = Config::get_app();

  int screen_width, screen_height;
  if (!camera_reader_->get_screen_size(screen_width, screen_height)) {
    SZ_LOG_ERROR("Get screen size error");
    throw std::runtime_error("Get screen size error");
  } else {
    SZ_LOG_INFO("Get screen w={}, h={}", screen_width, screen_height);
  }

  qRegisterMetaType<DetectionRatio>("DetectionRatio");
  qRegisterMetaType<PersonData>("PersonData");

  // 初始化QT
  QPalette pal = palette();
  pal.setColor(QPalette::Background, Qt::transparent);
  setPalette(pal);

  // 创建BGR人脸检测框控件
  detect_tip_widget_bgr_ =
      new DetectTipWidget(0, 0, screen_width, screen_height, this);
  detect_tip_widget_bgr_->hide();
  connect((const QObject *)detect_task_,
          SIGNAL(tx_bgr_display(DetectionRatio, bool, bool, bool)),
          (const QObject *)detect_tip_widget_bgr_,
          SLOT(rx_display(DetectionRatio, bool, bool, bool)));

  // 创建红外人脸检测框控件
  int pip_win_percent = app.infrared_window_percent;
  detect_tip_widget_nir_ =
      new DetectTipWidget(screen_width - screen_width * pip_win_percent / 100,
                          0, screen_width * pip_win_percent / 100,
                          screen_height * pip_win_percent / 100, this);
  detect_tip_widget_nir_->hide();
  if (app.show_infrared_window) {
    connect((const QObject *)detect_task_,
            SIGNAL(tx_nir_display(DetectionRatio, bool, bool, bool)),
            (const QObject *)detect_tip_widget_nir_,
            SLOT(rx_display(DetectionRatio, bool, bool, bool)));
  }

  // 创建人脸识别记录控件
  int x = 0;
  int y = screen_height * app.recognize_tip_top_percent / 100;
  int w = screen_width;
  int h = screen_height - y;

  recognize_tip_widget_ = new RecognizeTipWidget(nullptr);
  recognize_tip_widget_->hide();
  recognize_tip_widget_->setFixedSize(w, h);
  recognize_tip_widget_->move(x, y);
  connect((const QObject *)record_task_, SIGNAL(tx_display(PersonData, bool)),
          (const QObject *)recognize_tip_widget_,
          SLOT(rx_display(PersonData, bool)));

  // 创建屏保控件
  screen_saver_ = new ScreenSaverWidget(screen_width, screen_height, nullptr);
  screen_saver_->hide();
//   connect((const QObject *)face_timer_, SIGNAL(tx_face_disappear(int)),
//           (const QObject *)screen_saver_, SLOT(rx_display(int)));
//   connect((const QObject *)face_timer_, SIGNAL(tx_face_appear(int)),
//           (const QObject *)screen_saver_, SLOT(rx_hide()));

  // 创建人体轮廓控件
  outline_widget_ = new OutlineWidget(screen_width, screen_height, nullptr);

  // 创建顶部状态栏控件
  status_banner_ = new StatusBanner(screen_width, screen_height, nullptr);
}
