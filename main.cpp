#include <QMetaType>
#include <QtWidgets/QApplication>

#include "video_player.hpp"

using namespace suanzi;

int main(int argc, char *argv[]) {
  qRegisterMetaType<PersonDisplay>("PersonDisplay");
  qRegisterMetaType<DetectionFloat>("DetectionFloat");

  QApplication app(argc, argv);

  VideoPlayer player;
  player.show();

  return app.exec();
}
