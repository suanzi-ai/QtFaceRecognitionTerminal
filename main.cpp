#include <QMetaType>
#include <QTranslator>
#include <QtWidgets/QApplication>

#include "face_server.hpp"
#include "http_server.hpp"
#include "video_player.hpp"

using namespace suanzi;

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

  // 读取摄像头参数
  auto bgr_cam = Config::get_camera(CAMERA_BGR);
  auto nir_cam = Config::get_camera(CAMERA_NIR);

  // 读取应用参数
  auto app_cfg = Config::get_app();

  EngineOption opt = {
      .bgr =
          {
              .dev = bgr_cam.index,
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
                      {
                          .index = 1,
                          .rotate = 1,
                          .size =
                              {
                                  .width = 1080,
                                  .height = 704,
                              },
                      },
                      {
                          .index = 2,
                          .rotate = 1,
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
              .dev = nir_cam.index,
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
                      {
                          .index = 1,
                          .rotate = 1,
                          .size =
                              {
                                  .width = 1080,
                                  .height = 704,
                              },
                      },
                      {
                          .index = 2,
                          .rotate = 1,
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

VideoPlayer* create_gui() {
  // 加载 Quface 算法模块
  auto quface = Config::get_quface();
  auto detector = std::make_shared<FaceDetector>(quface.model_file_path);
  auto extractor = std::make_shared<FaceExtractor>(quface.model_file_path);
  auto pose_estimator =
      std::make_shared<FacePoseEstimator>(quface.model_file_path);
  auto anti_spoof = std::make_shared<FaceAntiSpoofing>(quface.model_file_path);
  auto db = std::make_shared<FaceDatabase>(quface.db_name);

  // 加载 Web 服务模块
  auto app_cfg = Config::get_app();
  static auto person_service = PersonService::make_shared(
      app_cfg.person_service_base_url, app_cfg.image_store_path);
  static auto face_service =
      std::make_shared<FaceService>(db, detector, pose_estimator, extractor,
                                    person_service, app_cfg.image_store_path);
  static auto face_server = std::make_shared<FaceServer>(face_service);
  static auto http_server = std::make_shared<HTTPServer>();
  face_server->add_event_source(http_server);

  std::thread t(
      [&]() { http_server->run(app_cfg.server_port, app_cfg.server_host); });
  t.detach();

  VideoPlayer* player = new VideoPlayer(db, detector, pose_estimator, extractor,
                                        anti_spoof, person_service);
  return player;
}

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  auto config = read_cfg(argc, argv);
  if (config == NULL) return -1;

  auto engine = create_engine();
  if (engine == NULL) return -1;

  // 播放开机画面
  // engine->start_boot_ui(":asserts/background.jpg");

  auto gui = create_gui();
  if (gui == NULL) return -1;
  gui->show();

  // 关闭开机画面
  // engine->stop_boot_ui();

  return app.exec();
}
