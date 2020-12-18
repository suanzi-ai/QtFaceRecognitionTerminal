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
  reader_timer_ = new QTimer(this);
  connect(reader_timer_, SIGNAL(timeout()), this, SLOT(read_co2()));
  reader_timer_->start(1000);

  moveToThread(this);
  start();
}

Co2Task::~Co2Task() {}

void Co2Task::run() { exec(); }

void Co2Task::read_co2() {}
