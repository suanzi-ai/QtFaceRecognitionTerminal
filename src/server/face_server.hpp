#pragma once

#include "event.hpp"
#include "face_service.hpp"

namespace suanzi {

class FaceServer {
 public:
  typedef std::shared_ptr<FaceServer> ptr;
  FaceServer(FaceService::ptr face_service) : face_service_(face_service) {}
  ~FaceServer() {}

  void add_event_source(EventEmitterPtr emitter);

 private:
  FaceService::ptr face_service_;
};

}  // namespace suanzi
