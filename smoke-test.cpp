#include <QFile>
#include <fstream>
#include <quface-io/engine.hpp>
#include <quface/logger.hpp>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "config.hpp"
#include "mpi_sys.h"

using namespace suanzi;
using namespace suanzi::io;

void test() {
  LCDScreenType screen_type = RH_9881_8INCH_800X1280;
  if (!Config::load_screen_type(screen_type)) return;

  SensorType sensor0_type = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT;
  SensorType sensor1_type = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT;
  if (!Config::load_sensor_type(sensor0_type, sensor1_type)) return;

  EngineOption opt = {
      .bgr =
          {
              .sensor_type = sensor0_type,
              .dev = 0,
              .flip = true,
              .channels =
                  {
                      {
                          .index = 0,
                          .rotate = 0,
                          .size =
                              {
                                  .width = 1920,
                                  .height = 1080,
                              },
                      },
                  },
          },
      .nir =
          {
              .sensor_type = sensor1_type,
              .dev = 1,
              .flip = true,
              .channels =
                  {
                      {
                          .index = 0,
                          .rotate = 0,
                          .size =
                              {
                                  .width = 1920,
                                  .height = 1080,
                              },
                      },
                  },
          },
      .screen =
          {
              .type = screen_type,
          },
      .show_secondary_win = true,
      .secondary_win_percent = SECONDARY_WIN_PERCENT_50,
  };

  auto engine = Engine::instance();

  engine->set_option(opt);
  engine->start();

  {
    for (int i = 0; i < 5; i++) {
      SZ_LOG_INFO("Turn on light box ... {}", i);
      engine->gpio_set(GpioPinLightBox, true);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      SZ_LOG_INFO("Turn off light box ... {}", i);
      engine->gpio_set(GpioPinLightBox, false);
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
  }

  {
    std::string name = ":asserts/zh-CN/recognition_succeed.aac";
    std::vector<SZ_BYTE> audio;
    QFile audio_file(name.c_str());
    if (!audio_file.open(QIODevice::ReadOnly)) {
      SZ_LOG_ERROR("Open {} failed", name);
    }

    auto data = audio_file.readAll();
    audio.assign(data.begin(), data.end());

    engine->audio_set_volume(100);

    for (int i = 0; i < 5; i++) {
      SZ_LOG_INFO("Play audio ... {}", i);
      engine->audio_play(audio);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
  }
}

int main() {
  HI_MPI_SYS_Init();
  test();
  std::this_thread::sleep_for(std::chrono::milliseconds(60000));
  HI_MPI_SYS_Exit();
  return 0;
}
