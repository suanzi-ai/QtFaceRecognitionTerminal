#include "face_server.hpp"
#include "http_server.hpp"

using namespace suanzi;

int main(int argc, char *argv[]) {
  std::string model = "facemodel.bin";
  std::string storePath = "/user/xzp-home/go-app/upload/";
  if (argc > 2) {
    model = argv[1];
    storePath = argv[2];
  }

  auto detector = std::make_shared<FaceDetector>(model);
  auto extractor = std::make_shared<FaceExtractor>(model);
  auto db = std::make_shared<FaceDatabase>("quface");

  auto face_service = std::make_shared<FaceService>(db, detector, extractor, storePath);
  auto face_server = std::make_shared<FaceServer>(face_service);

  auto http_server = std::make_shared<HTTPServer>();

  face_server->add_event_source(http_server);

  http_server->run(8009);

  return 0;
}
