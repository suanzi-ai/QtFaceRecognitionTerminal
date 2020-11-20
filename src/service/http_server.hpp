#pragma once

#include <httplib.h>

#include <quface/common.hpp>
#include <quface/logger.hpp>

#include "config.hpp"
#include "event.hpp"

namespace suanzi {
using namespace httplib;

class HTTPServer : public EventEmitter {
 public:
  typedef std::shared_ptr<HTTPServer> ptr;

  HTTPServer(bool enable_logger = false);

  void run(uint16_t port, const std::string& host = "0.0.0.0");

 private:
  void response_failed(Response& res, const std::string& message);
  void response_ok(Response& res);

  std::shared_ptr<Server> server_;
};
}  // namespace suanzi
