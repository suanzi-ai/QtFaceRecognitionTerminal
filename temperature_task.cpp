#include <string>

#include <QtWidgets/QApplication>

#include <quface-io/engine.hpp>

#include "config.hpp"
#include "temperature_task.hpp"

using namespace suanzi;
using namespace suanzi::io;

Config* read_cfg(int argc, char* argv[]) {
  // 基础配置文件，默认：config.json
  std::string cfg_file = "config.json";

  // 动态加载配置文件，默认：config.override.json
  std::string cfg_override_file = "config.override.json";

  // 读取命令行参数，格式为
  // ./face-terminal --config config.json --override-config config.override.json
  for (int i = 1; i < argc; i++) {
    auto arg = std::string(argv[i]);
    if (i < argc - 1) {
      if (arg == "-c" || arg == "--config") {
        i++;
        cfg_file = argv[i];
      } else if (arg == "-cc" || arg == "--override-config") {
        i++;
        cfg_override_file = argv[i];
      }
    }
  }

  // 加载配置文件
  auto config = Config::get_instance();
  if (SZ_RETCODE_OK == config->load_from_file(cfg_file, cfg_override_file))
    return config;
  else
    return NULL;
}

Engine* create_engine() {
  // 读取屏幕类型
  LCDScreenType lcd_screen_type;
  if (!Config::load_screen_type(lcd_screen_type)) return NULL;

  SensorType sensor0_type = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT;
  SensorType sensor1_type = SONY_IMX327_2L_MIPI_2M_30FPS_12BIT;
  if (!Config::load_sensor_type(sensor0_type, sensor1_type)) return NULL;

  // 读取摄像头参数
  auto bgr_cam = Config::get_camera(CAMERA_BGR);
  auto nir_cam = Config::get_camera(CAMERA_NIR);

  // 读取应用参数
  auto app_cfg = Config::get_app();

  EngineOption opt = {
      .bgr =
          {
              .sensor_type = sensor0_type,
              .dev = bgr_cam.index,
              .flip = true,
              .wdr = false,
              .channels =
                  {
                      {
                          .index = 0,
                          .rotate = bgr_cam.rotate,
                          .size =
                              {
                                  .width = 1920,
                                  .height = 1080,
                              },
                      },
                      {
                          .index = 1,
                          .rotate = bgr_cam.rotate,
                          .size =
                              {
                                  .width = 1080,
                                  .height = 704,
                              },
                      },
                      {
                          .index = 2,
                          .rotate = bgr_cam.rotate,
                          .size =
                              {
                                  .width = 320,
                                  .height = 224,
                              },
                      },
                  },
          },
      .nir =
          {
              .sensor_type = sensor1_type,
              .dev = nir_cam.index,
              .flip = true,
              .wdr = false,
              .channels =
                  {
                      {
                          .index = 0,
                          .rotate = nir_cam.rotate,
                          .size =
                              {
                                  .width = 1920,
                                  .height = 1080,
                              },
                      },
                      {
                          .index = 1,
                          .rotate = nir_cam.rotate,
                          .size =
                              {
                                  .width = 1080,
                                  .height = 704,
                              },
                      },
                      {
                          .index = 2,
                          .rotate = nir_cam.rotate,
                          .size =
                              {
                                  .width = 320,
                                  .height = 224,
                              },
                      },
                  },
          },
      .screen =
          {
              .type = lcd_screen_type,
          },
      .show_secondary_win = app_cfg.show_infrared_window,
      .secondary_win_percent =
          (SecondaryWinPercent)app_cfg.infrared_window_percent,
  };

  auto engine = Engine::instance();
  engine->set_option(opt);

  return engine;
}

int main(int argc, char* argv[]) {
  // Step 1: 加载配置文件
  auto config = read_cfg(argc, argv);
  if (config == NULL) return -1;

  // Step 2: 必须先创建Engine
  auto engine = create_engine();
  if (engine == NULL) return -1;

  // Step 3: 初始化QT（必须在Engine初始化之后）
  QApplication app(argc, argv);

  // Step 4: 启动temperature task
  TemperatureTask::get_instance();

  return app.exec();
}
