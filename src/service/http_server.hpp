#pragma once

#include <httplib.h>

#include <quface/common.hpp>

#include "config.hpp"
#include "event.hpp"
#include "logger.hpp"

namespace suanzi {
using namespace httplib;

class HTTPServer : public EventEmitter {
 public:
  typedef std::shared_ptr<HTTPServer> ptr;

  HTTPServer(Config::ptr config);

  void run(uint16_t port, const std::string &host = "0.0.0.0");

 private:
  std::shared_ptr<Server> server_;
  Config::ptr config_;
};
}  // namespace suanzi
