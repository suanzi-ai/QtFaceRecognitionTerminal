#include "video_player.hpp"

#include <QBoxLayout>
#include <QDebug>
#include <QPainter>
#include <QPushButton>
#include <QTimer>

using namespace suanzi;

VideoPlayer::VideoPlayer(FaceDatabasePtr db, FaceDetectorPtr detector,
                         FaceExtractorPtr extractor, QWidget *parent)
    : QWidget(parent) {
  QPalette pal = palette();
  pal.setColor(QPalette::Background, Qt::transparent);
  pal.setColor(QPalette::Foreground, Qt::green);
  setPalette(pal);

  auto app = Config::get_app();
  auto quface = Config::get_quface();

  detect_tip_widget_bgr_ =
      new DetectTipWidget(0, 0, app.window_width, app.window_height, this);
  detect_tip_widget_bgr_->hide();

  detect_tip_widget_nir_ = new DetectTipWidget(
      app.window_width - app.window_width * 25 / 100, 0,
      app.window_width * 25 / 100, app.window_height * 25 / 100, this);
  detect_tip_widget_nir_->hide();

  recognize_tip_widget_ = new RecognizeTipWidget(nullptr);
  recognize_tip_widget_->hide();

  camera_reader_ = new CameraReader(this);

  auto person_service = PersonService::make_shared(app.person_service_base_url,
                                                   app.image_store_path);
  auto anti_spoofing =
      std::make_shared<FaceAntiSpoofing>(quface.model_file_path);

  detect_task_ = new DetectTask(detector, nullptr, this);
  recognize_task_ = new RecognizeTask(db, extractor, person_service, &mem_pool_,
                                      nullptr, this);
  record_task_ = new RecordTask(person_service, &mem_pool_);
  anti_spoofing_task_ = new AntiSpoofingTask(anti_spoofing, nullptr, this);

  connect((const QObject *)camera_reader_,
          SIGNAL(tx_frame(PingPangBuffer<ImagePackage> *)),
          (const QObject *)detect_task_,
          SLOT(rx_frame(PingPangBuffer<ImagePackage> *)));

  connect((const QObject *)detect_task_, SIGNAL(tx_finish()),
          (const QObject *)camera_reader_, SLOT(rx_finish()));

  connect((const QObject *)detect_task_, SIGNAL(tx_bgr_display(DetectionRatio)),
          (const QObject *)detect_tip_widget_bgr_,
          SLOT(rx_display(DetectionRatio)));

  connect((const QObject *)detect_task_, SIGNAL(tx_nir_display(DetectionRatio)),
          (const QObject *)detect_tip_widget_nir_,
          SLOT(rx_display(DetectionRatio)));

  connect((const QObject *)detect_task_,
          SIGNAL(tx_frame(PingPangBuffer<RecognizeData> *)),
          (const QObject *)anti_spoofing_task_,
          SLOT(rx_frame(PingPangBuffer<RecognizeData> *)));

  connect((const QObject *)detect_task_, SIGNAL(tx_no_frame()),
          (const QObject *)anti_spoofing_task_, SLOT(rx_no_frame()));

  connect((const QObject *)anti_spoofing_task_, SIGNAL(tx_finish()),
          (const QObject *)detect_task_, SLOT(rx_finish()));

  connect((const QObject *)anti_spoofing_task_,
          SIGNAL(tx_frame(PingPangBuffer<RecognizeData> *)),
          (const QObject *)recognize_task_,
          SLOT(rx_frame(PingPangBuffer<RecognizeData> *)));

  connect((const QObject *)anti_spoofing_task_, SIGNAL(tx_no_frame()),
          (const QObject *)recognize_task_, SLOT(rx_no_frame()));

  connect((const QObject *)recognize_task_, SIGNAL(tx_finish()),
          (const QObject *)anti_spoofing_task_, SLOT(rx_finish()));

  connect((const QObject *)recognize_task_, SIGNAL(tx_display(PersonData)),
          (const QObject *)recognize_tip_widget_, SLOT(rx_display(PersonData)));

  connect((const QObject *)recognize_task_,
          SIGNAL(tx_record(int, ImageBuffer *)), (const QObject *)record_task_,
          SLOT(rx_record(int, ImageBuffer *)));

  camera_reader_->start_sample();

  QTimer::singleShot(1, this, SLOT(init_widgets()));
}

VideoPlayer::~VideoPlayer() {}

void VideoPlayer::paintEvent(QPaintEvent *event) {
  QPainter painter(this);
  detect_tip_widget_bgr_->paint(&painter);
  detect_tip_widget_nir_->paint(&painter);
}

void VideoPlayer::init_widgets() {
  auto app = Config::get_app();

  int x = 0;
  int y = app.window_height * app.recognize_tip_top_percent / 100;
  int w = app.window_width;
  int h = app.window_height - y;

  recognize_tip_widget_->setFixedSize(w, h);
  recognize_tip_widget_->move(x, y);
  recognize_tip_widget_->show();
}
