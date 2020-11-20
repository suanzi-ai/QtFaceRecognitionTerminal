#include "face_server.hpp"

using namespace suanzi;

void FaceServer::add_event_source(EventEmitterPtr emitter) {
  emitter->appendListener("db.add", [&](EventData &body, ResultCallback cb) {
    cb(face_service_->db_add(body));
  });

  emitter->appendListener("db.add_many",
                          [&](EventData &body, ResultCallback cb) {
                            cb(face_service_->db_add_many(body));
                          });

  emitter->appendListener("db.remove_by_id",
                          [&](EventData &body, ResultCallback cb) {
                            cb(face_service_->db_remove_by_id(body));
                          });

  emitter->appendListener("db.remove_all",
                          [&](EventData &body, ResultCallback cb) {
                            cb(face_service_->db_remove_all(body));
                          });

  emitter->appendListener("db.get_all",
                          [&](EventData &body, ResultCallback cb) {
                            cb(face_service_->db_get_all(body));
                          });
}
