#pragma once

#include <httplib.h>

#include <quface/common.hpp>

#include "event.hpp"
#include "logger.hpp"

namespace suanzi {
using namespace httplib;

class HTTPServer : public EventEmitter {
 public:
  typedef std::shared_ptr<HTTPServer> ptr;

  HTTPServer();

  void run(uint16_t port);

 private:
  std::shared_ptr<Server> m_svr;
};
}  // namespace suanzi
