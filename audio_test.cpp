#include "audio.hpp"

#include <thread>
#include <unistd.h>

void test() {
  suanzi::AudioPlayer player;
  bool done = false;

  std::thread([&]() {
    bool bswitch = true;
    while (!done) {
      player.play("telephone.aac");

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

int main(int argc, char* argv[]) {
  HI_MPI_SYS_Init();
  test();
  HI_MPI_SYS_Exit();
}
