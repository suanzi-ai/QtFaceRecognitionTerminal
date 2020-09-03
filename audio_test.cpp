#include "quface-io/lib/audio/audio.hpp"

#include <thread>
#include <unistd.h>

using namespace suanzi;

void test() {
  auto player = suanzi::AudioPlayer::instance();
  bool done = false;

  std::thread([&]() {
    bool bswitch = true;
    while (!done) {
      player->play(":asserts/success.aac");

      if (bswitch) {
        player->set_volume(6);
      } else {
        player->set_volume(-20);
      }
      bswitch = !bswitch;
    }
  }).detach();
  SZ_LOG_INFO("Press any key to exit");
  getchar();
  done = true;
  sleep(1);
}

int main(int argc, char* argv[]) {
  HI_MPI_SYS_Init();
  test();
  HI_MPI_SYS_Exit();
}
