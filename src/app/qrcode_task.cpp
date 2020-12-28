#include "qrcode_task.hpp"
#include <zbar.h>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <quface-io/engine.hpp>
#include "config.hpp"
#include "qrcode_task.hpp"

using namespace suanzi;
using namespace suanzi::io;

std::string zbar_decoder(const cv::Mat& img) {
  zbar::ImageScanner scanner;
  const void* raw = img.data;
  // configure the reader
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 1);
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_X_DENSITY, 1);
  scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_Y_DENSITY, 1);

  // wrap image data
  zbar::Image image(img.cols, img.rows, "Y800", raw, img.cols * img.rows);
  // scan the image for barcodes
  int n = scanner.scan(image);
  // extract results

  zbar::Image::SymbolIterator symbol = image.symbol_begin();
  std::string result = symbol->get_data();
  image.set_data(NULL, 0);
  return result;
}

std::string return_code_from_binary_image(const cv::Mat& image) {
  std::string result = zbar_decoder(image);
  if (result.empty()) {
    cv::Mat open_img;
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(image, open_img, cv::MORPH_OPEN, element);
    result = zbar_decoder(open_img);
  }
  return result;
}

std::string reture_code(const cv::Mat& image) {
  static cv::Mat binary_image;

  cv::adaptiveThreshold(image, binary_image, 255,
                        cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 33,
                        0);

  return return_code_from_binary_image(binary_image);
}

bool QrcodeTask::scan_qrcode() {
  const int screen_width = 800;
  const int screen_height = 1280;
  const int QRCODE_WIDTH = 400;
  const int QRCODE_HEIGHT = 400;

  auto engine = Engine::instance();

  static MmzImage* mmz_yuv_image = NULL;
  if (mmz_yuv_image == NULL)
    mmz_yuv_image = new MmzImage(1072, 1728, SZ_IMAGETYPE_NV21);

  if (SZ_RETCODE_OK ==
      engine->capture_frame(io::CAMERA_BGR, 0, *mmz_yuv_image)) {
    static cv::Mat qr_image;
    static int index = 0;

    cv::Mat yuvImage(mmz_yuv_image->height, mmz_yuv_image->width, CV_8UC1,
                     mmz_yuv_image->pData);
    /*yuvImage(cv::Rect((screen_width - QRCODE_WIDTH) * 1.35 / 2,
                      (screen_height - QRCODE_HEIGHT) * 1.35 / 2,
                      QRCODE_WIDTH * 1.35, QRCODE_HEIGHT * 1.35))
        .copyTo(qr_image);*/

    yuvImage(cv::Rect((screen_width - QRCODE_WIDTH) * 1.34 / 2, 100 * 1.35,
                      QRCODE_WIDTH * 1.34, QRCODE_HEIGHT * 1.35))
        .copyTo(qr_image);

    cv::flip(qr_image, qr_image, 1);

    std::string qr_code = reture_code(qr_image);
    if (!qr_code.empty()) {
      printf("%s\n", qr_code.c_str());
      emit tx_ok();
      emit tx_detect_result(true);
      emit tx_qrcode(QString(qr_code.c_str()));
      return true;
    } else {
      emit tx_detect_result(false);
    }
  }

  return false;
}

QrcodeTask* QrcodeTask::get_instance() {
  static QrcodeTask instance;
  return &instance;
}

QrcodeTask::QrcodeTask(QThread* thread, QObject* parent) {
  person_service_ = PersonService::get_instance();

  reader_timer_ = new QTimer(this);
  connect(reader_timer_, SIGNAL(timeout()), this, SLOT(read_qrcode()));

  moveToThread(this);
  start();
}

QrcodeTask::~QrcodeTask() {}

void QrcodeTask::read_qrcode() { scan_qrcode(); }

void QrcodeTask::run() { exec(); }

void QrcodeTask::start_qrcode_task() {
  reader_timer_->start(250);
  emit stop_read_cameral();
}

void QrcodeTask::stop_qrcode_task() {
  reader_timer_->stop();
  emit start_read_cameral();
}
