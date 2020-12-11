#include "qrcode_widget.hpp"
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTimer>
#include <QDebug>
#include <quface-io/mmzimage.hpp>
#include <quface/common.hpp>
#include <quface-io/engine.hpp>
#include <quface-io/ive.hpp>



#include "config.hpp"
//#include "JQQRCodeReader/JQQRCodeReader.h"
#include "zbar.h"


using namespace suanzi;
using namespace suanzi::io;
using namespace zbar;
QrcodeWidget::QrcodeWidget(int screen_width, int screen_height, QWidget *parent)
    : screen_width_(screen_width), screen_height_(screen_height), QWidget(parent) {

  QPushButton *ppb_back = new QPushButton(this);
  ppb_back->setFixedSize(150, 150);
  ppb_back->setStyleSheet("QPushButton {border-image: url(:asserts/back_small.png);}");
  ppb_back->setFocusPolicy(Qt::NoFocus);

  QVBoxLayout *pv_layout = new QVBoxLayout;
  pv_layout->addSpacing(QRCODE_HEIGHT + 170);
  pv_layout->addWidget(ppb_back, 0, Qt::AlignCenter);
  pv_layout->setMargin(0);
  setLayout(pv_layout);

  connect(ppb_back, SIGNAL(pressed()), this, SLOT(clicked_back()));

  timer_ = new QTimer(this);
  connect(timer_, SIGNAL(timeout()), this, SLOT(scan_qrcode_ui()));

  pl_lightneedle_ = new QLabel(this);
  pl_lightneedle_->setFixedSize(QRCODE_WIDTH - 10, 48);
  pl_lightneedle_->setStyleSheet("QLabel {border-image: url(:asserts/lightneedle.png);}");
  pos_x_ = (screen_width - QRCODE_WIDTH) / 2 + 2;
  pl_lightneedle_->hide();

  bstart_scan_qrcode_ = false;
  static ScanQrcodeTask scan_qrcode_task(this);
}

QrcodeWidget::~QrcodeWidget() {}


QrcodeWidget::ScanQrcodeTask::ScanQrcodeTask(QObject *parent) : QThread(parent) {
	start();
}

void QrcodeWidget::ScanQrcodeTask::run()  {
	while(1) {

		QrcodeWidget *qrwidget = (QrcodeWidget *)parent();
		if (qrwidget->bstart_scan_qrcode_)
			qrwidget->scan_qrcode();
		else
			QThread::usleep(250);
	}
}

void QrcodeWidget::init() {
	//adjustSize();
	timer_->start(50);
  	pos_y_ = 100 + 2;
	pl_lightneedle_->move(pos_x_, pos_y_);
	pl_lightneedle_->show();
	bstart_scan_qrcode_ = true;
}


std::string QrcodeWidget::zbar_decoder(cv::Mat &img) {

    ImageScanner scanner;
    const void *raw = img.data;
    // configure the reader
    scanner.set_config(ZBAR_NONE, ZBAR_CFG_ENABLE, 1);
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_X_DENSITY, 1);
	scanner.set_config(ZBAR_NONE, ZBAR_CFG_Y_DENSITY, 1);
    // wrap image data
    //printf("w=%d h=%d\n", img.cols, img.rows);
    Image image(img.cols, img.rows, "Y800", raw, img.cols * img.rows);
    // scan the image for barcodes
    int n = scanner.scan(image);
    // extract results

	Image::SymbolIterator symbol = image.symbol_begin();
    std::string result = symbol->get_data();
	/*printf("decodestr=%s \n", result.c_str());
	for (int i = 0; i < 4; i++)
	{
		std::cout << "(" << symbol->get_location_x(i) << "," << symbol->get_location_y(i) << ")";
	}
	std::cout << std::endl;*/
    image.set_data(NULL, 0);
    return result;
}


QString QrcodeWidget::get_qrcode(cv::Mat &img) {
    cv::Mat bin_img, adaptive_img;
    //在otsu二值结果的基础上，不断增加阈值，用于识别模糊图像
    //int thre = cv::threshold(img, bin_img, 0, 255, cv::THRESH_BINARY);
    //std::string result = get_qrcode_in_binimg(bin_img);

	//threshold第一个参数即原图像必须为灰度图,最佳33
	cv::adaptiveThreshold(img, adaptive_img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 33, 0);
    std::string result = get_qrcode_in_binimg(adaptive_img);
	if (result.empty()) {

	#if 0
		//for (int i = 61; i < 80; i+=2)
		{
			int i = 51;
			cv::adaptiveThreshold(img, adaptive_img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, i, 0);
			result = get_qrcode_in_binimg(adaptive_img);
			if (!result.empty()) {
		    	printf("blk = %d here0 %s\n", i, result.c_str());
				//break;
			} /*else {

				i = 91;
				cv::adaptiveThreshold(img, adaptive_img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, i, 0);
				result = get_qrcode_in_binimg(adaptive_img);
				if (!result.empty()) {
			    	printf("blk = %d here0 %s\n", i, result.c_str());
					//break;
				} else {
					i = 121;
					cv::adaptiveThreshold(img, adaptive_img, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, i, 0);
					result = get_qrcode_in_binimg(adaptive_img);
					if (!result.empty()) {
				    	printf("blk = %d here0 %s\n", i, result.c_str());
						//break;
					}
				}

			}*/
		}
	#endif
	} else {
		printf("here2 %s\n", result.c_str());
	}

	int thre = 110;
    while (result.empty() && thre < 110)
    {
        cv::threshold(img, bin_img, thre, 255, cv::THRESH_BINARY);
        result = get_qrcode_in_binimg(bin_img);

		if (!result.empty())
	    	printf("thr=%d result=%s\n", thre, result.c_str());

        thre += 20;//阈值步长设为20，步长越大，识别率越低，速度越快
    }
    QString strRes = QString(result.c_str());
    return strRes;
}

std::string QrcodeWidget::get_qrcode_in_binimg(cv::Mat &bin_img)
{
    std::string result = zbar_decoder(bin_img);
    if (result.empty()) {
        cv::Mat open_img;
        cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
        cv::morphologyEx(bin_img, open_img, cv::MORPH_OPEN, element);
        result = zbar_decoder(open_img);
		if (!result.empty()) {
		    	printf("here1 %s\n",  result.c_str());
		}
    }
    return result;
}


void QrcodeWidget::scan_qrcode_ui() {
	const int max_pos_y = 100 + QRCODE_HEIGHT - pl_lightneedle_->height() - 2;
	pos_y_ += 10;
	if (pos_y_ > max_pos_y)
		pos_y_ = 100 + 2;
	pl_lightneedle_->move(pos_x_, pos_y_);
}


void QrcodeWidget::scan_qrcode() {

	auto engine = Engine::instance();
	static MmzImage *mmz_yuv_image = NULL;
	static MmzImage *mmz_bgr_image = NULL;
	if (mmz_yuv_image == NULL) {
		Size bgr_size;
		engine->get_frame_size(CAMERA_BGR, 0, bgr_size);
		mmz_yuv_image = new MmzImage(1072, 1728, SZ_IMAGETYPE_NV21);
		mmz_bgr_image = new MmzImage(1072, 1728, SZ_IMAGETYPE_BGR);
	}

	SZ_RETCODE ret = engine->capture_frame(io::CAMERA_BGR, 0, *mmz_yuv_image);
	if (ret == SZ_RETCODE_OK) {
		//qDebug() << "get frame\n";
		//qDebug() << mmz_bgr_image->width << mmz_bgr_image->height;
/*
		auto ive = Ive::getInstance();
  		if (!ive->yuv2RgbPlannar(mmz_bgr_image, mmz_yuv_image, true))
			return;*/

		cv::Mat yuvImage(mmz_yuv_image->height, mmz_yuv_image->width, CV_8UC1, mmz_yuv_image->pData);
		//cv::flip(yuvImage, yuvImage, 1);
		//cv::Mat input_image(mmz_yuv_image->height, mmz_yuv_image->width, CV_8UC3);
		//cvtColor(yuvImage, input_image, CV_YUV2BGR_NV21);

  		//cv::Mat input_image(mmz_bgr_image->height, mmz_bgr_image->width, CV_8UC3, (unsigned char *)mmz_bgr_image->pData);
  		cv::Mat qr_image = yuvImage(cv::Rect((screen_width_ - QRCODE_WIDTH) * 1.34 / 2, /*(screen_height_ - QRCODE_HEIGHT)* 1.35 / 2*/100 * 1.35,
             QRCODE_WIDTH * 1.34, QRCODE_HEIGHT * 1.35));

		cv::flip(qr_image, qr_image, 1);
		cv::rectangle(yuvImage, cv::Rect((screen_width_ - QRCODE_WIDTH) * 1.34 / 2, 100 * 1.35,
             QRCODE_WIDTH * 1.34, QRCODE_HEIGHT * 1.35), cv::Scalar(0,0,255));
		 static int index = 0;
				//cv::imwrite(QString::number(index++).toStdString() + ".png", qr_image);
				if (index > 30)
					index = 0;
				index++;
#if 0
		cv::rectangle(input_image, cv::Rect((screen_width_ - QRCODE_WIDTH) * 1.35 / 2, (screen_height_ - QRCODE_HEIGHT) * 1.35 / 2,
             QRCODE_WIDTH * 1.35, QRCODE_HEIGHT * 1.35), cv::Scalar(0,0,255));
	    static int index = 0;
		cv::imwrite(QString::number(index++).toStdString() + ".png", qr_image);
		if (index > 30)
			index = 0;

	    QImage image((unsigned char *)qr_image.data,
               qr_image.cols, qr_image.rows,
               qr_image.step, QImage::Format_RGB888);
		//const QImage image("/mnt/15.png");
		JQQRCodeReader qrCodeReader;
		QString str = qrCodeReader.decodeImage( image, JQQRCodeReader::DecodeQrCodeType );
		//if (!str.isEmpty())
			qDebug() << str;
#else
		//qr_image = cv::imread("/mnt/11.png", 0);
		get_qrcode(qr_image);
#endif
	}
}


void QrcodeWidget::paintEvent(QPaintEvent *event) {
	QPainter painter(this);
	QPen pen = painter.pen();
	pen.setWidth(5);
	pen.setBrush(QColor(2, 168, 223));
	painter.setPen(pen);

	float width = QRCODE_WIDTH;
	float height = QRCODE_HEIGHT;
	int x = (screen_width_ - QRCODE_WIDTH) / 2;
  	int y = 100;//(screen_height_ - QRCODE_HEIGHT) / 2;
	float top_x = x;
	float top_y = y;
	float bottom_x = top_x + width;
	float bottom_y = top_y + height;

	// SZ_LOG_INFO("top_x={},top_y={},bottom_x={},bottom_y={},w={},h={}", top_x,
	//              top_y, bottom_x, bottom_y, width, height);

	painter.drawLine(top_x, top_y, top_x + width / 5, top_y);
	painter.drawLine(top_x, top_y, top_x, top_y + height / 5);
	painter.drawLine(top_x, bottom_y, top_x + width / 5, bottom_y);
	painter.drawLine(top_x, bottom_y, top_x, bottom_y - height / 5);
	painter.drawLine(bottom_x, top_y, bottom_x, top_y + height / 5);
	painter.drawLine(bottom_x, top_y, bottom_x - width / 5, top_y);
	painter.drawLine(bottom_x, bottom_y, bottom_x, bottom_y - height / 5);
	painter.drawLine(bottom_x, bottom_y, bottom_x - width / 5, bottom_y);
}


void QrcodeWidget::clicked_back() {
	timer_->stop();
	bstart_scan_qrcode_ = false;
	emit switch_stacked_widget(0);
}
