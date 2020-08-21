#include "audio.hpp"

#include <thread>
#include <unistd.h>
#include "platform_interface.h"
#include <QObject>


using namespace suanzi;

void test() {
  suanzi::AudioPlayer player;
  bool done = false;

  std::thread([&]() {
    bool bswitch = true;
    while (!done) {
      player.play(":asserts/success.aac");

      if (bswitch) {
        player.set_volume(6);
      } else {
        player.set_volume(-20);
      }
      bswitch = !bswitch;
    }
  }).detach();
  SZ_LOG_INFO("Press any key to exit");
  getchar();
  done = true;
  sleep(1);
}


#if 0
class TestTemp: public QObject {
	
	public:
		TestTemp() {
			static ObjectTempTask obj_temp_task;
			static AmbientTempTask ambient_task;
		    connect((const QObject *)&ambient_task, SIGNAL(tx_ambient_temp(float)),
		          (const QObject *)&obj_temp_task, SLOT(rx_ambient_temp(float)));
		}
};

#endif

#include <QtWidgets/QApplication>

int main(int argc, char* argv[]) {
#if 0
  HI_MPI_SYS_Init();
  test();
  HI_MPI_SYS_Exit();
#endif
	//TestTemp test_temp;
	//getchar();
	//QApplication app(argc, argv);
	//return app.exec();
}
