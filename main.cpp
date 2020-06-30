#include <QtWidgets/QApplication>
#include "videoplayer.h"
#include <QMetaType>

int main(int argc, char *argv[])
{

    qRegisterMetaType<Person>("Person");
    QApplication app(argc, argv);

    VideoPlayer player;
    player.show();

    return app.exec();
}
