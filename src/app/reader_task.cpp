#include "reader_task.hpp"

#include <quface-io/engine.hpp>

using namespace suanzi;
using namespace suanzi::io;

ReaderTask::ReaderTask() {
  auto engine = Engine::instance();
  ic_reader_ = engine->get_ic_reader();
  if (ic_reader_ == nullptr) {
    SZ_LOG_ERROR("Get temperature reader error");
    return;
  }

  start();
}

ReaderTask::~ReaderTask() {}

void ReaderTask::rx_enable() { is_enabled_ = true; }

void ReaderTask::rx_disable() { is_enabled_ = false; }

void ReaderTask::run() {
  unsigned char card_no[15];
  int card_no_len = 0;
  is_enabled_ = true;
  while (true) {
    if (is_enabled_) {
      SZ_RETCODE ret = ic_reader_->read_card_no(card_no, card_no_len);
      if (ret == SZ_RETCODE_OK) {
      }
    }
    usleep(250000);
  }
}
