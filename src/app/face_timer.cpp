#include "face_timer.hpp"
#include <quface-io/engine.hpp>

#include "config.hpp"
#include "led_task.hpp"

using namespace suanzi;
using namespace suanzi::io;

FaceTimer *FaceTimer::get_instance() {
  static FaceTimer instance;
  return &instance;
}

FaceTimer::FaceTimer(QObject *parent) {

  screen_saver_timer_ = new QTimer(this);
  screen_saver_timer_->setSingleShot(true);
  connect(screen_saver_timer_, SIGNAL(timeout()), this, SLOT(screen_saver_timeout()));

  white_led_timer_ = new QTimer(this);
  white_led_timer_->setSingleShot(true);
  connect(white_led_timer_, SIGNAL(timeout()), this, SLOT(white_led_timeout()));


  disappear_counter_ = 0;
  disappear_duration_ = 0;
  int screensaver_timeout = Config::get_user().screensaver_timeout;
  screen_saver_timer_->start(screensaver_timeout * 1000);
  start();
}

FaceTimer::~FaceTimer() {}

void FaceTimer::rx_detect_result(bool valid_detect) {
	if (valid_detect) {
		bclose_white_led_ = false;
		LEDTask::get_instance()->turn_on();
		white_led_timer_->start(MAX_WHITE_LED_TIMEOUT * 1000);
		display_screen_saver(false);
	}
}


void FaceTimer::display_screen_saver(bool visible) {
	if (Config::get_user().enable_screensaver) {
		emit tx_display_screen_saver(visible);
	}
}

void FaceTimer::screen_saver_timeout() {
	display_screen_saver(true);
}

void FaceTimer::white_led_timeout() {
	emit tx_white_led_timeout();
	bclose_white_led_ = true;
	QTimer::singleShot(500, this, SLOT(delay_close_white_led()));
	int screensaver_timeout = Config::get_user().screensaver_timeout;
	if (MAX_WHITE_LED_TIMEOUT >= screensaver_timeout)
		display_screen_saver(true);
	else {
		screen_saver_timer_->start((screensaver_timeout - MAX_WHITE_LED_TIMEOUT) * 1000);
	}
}


void FaceTimer::delay_close_white_led() {
	if (bclose_white_led_)
		LEDTask::get_instance()->turn_off();
}

void FaceTimer::run() {
	while(1) usleep(1);
	exec();
}
