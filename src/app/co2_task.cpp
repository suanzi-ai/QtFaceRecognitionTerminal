#include "co2_task.hpp"
#include <cmath>
#include <quface-io/engine.hpp>
#include "config.hpp"

using namespace suanzi;
using namespace suanzi::io;

Co2Task *Co2Task::get_instance() {
  static Co2Task instance;
  return &instance;
}

Co2Task::Co2Task(QObject *parent) : QThread(parent) {
  auto engine = Engine::instance();
  co2_reader_ = engine->get_co2_reader();
  if (co2_reader_ == nullptr) {
    SZ_LOG_ERROR("Get co2 reader error");
    return;
  }

  reader_timer_ = new QTimer(this);
  connect(reader_timer_, SIGNAL(timeout()), this, SLOT(read_co2()));
  reader_timer_->start(250);

  moveToThread(this);
  start();
}

Co2Task::~Co2Task() {}

void Co2Task::run() { exec(); }

void Co2Task::read_co2() {
  int value = 0;
  if (SZ_RETCODE_OK == co2_reader_->read(value)) {
    emit tx_co2(value);
  }
}
