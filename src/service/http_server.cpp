#include "http_server.hpp"

#include <quface-io/engine.hpp>

#include "static_config.hpp"

using namespace suanzi;
using namespace suanzi::io;

HTTPServer::HTTPServer() {
  server_ = std::make_shared<Server>();

  server_->set_logger([](const Request& req, const Response& res) {
    // SZ_LOG_INFO("HTTP {} {} {} {}", req.method, req.path, req.content_length,
    //             res.status);
  });

  server_->set_error_handler([](const Request& req, Response& res) {
    auto fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
    char buf[BUFSIZ];
    snprintf(buf, sizeof(buf), fmt, res.status);
    res.set_content(buf, "text/html");
  });
}

void HTTPServer::run(uint16_t port, const std::string& host) {
  SZ_LOG_INFO("Http server license on port {}", port);

  auto handler = [&](const Request& req, Response& res) {
    if (!req.has_header("Content-Type")) {
      json data = {{"ok", false}, {"message", "Content-Type missing"}};
      res.set_content(data.dump(), "application/json");
      return;
    }
    if (req.get_header_value("Content-Type") != "application/json") {
      json data = {{"ok", false},
                   {"message", "content type shoule be application/json"}};
      res.set_content(data.dump(), "application/json");
      return;
    }

    auto method = "db." + req.matches[1].str();
    // SZ_LOG_DEBUG("method={}, body={}", method, req.body);
    auto bodyStr = req.body;
    if (bodyStr.size() == 0) {
      bodyStr = "{}";
    }

    json body;
    try {
      body = json::parse(bodyStr);
    } catch (const std::exception& exc) {
      json data = {{"ok", false}, {"message", exc.what()}};
      res.set_content(data.dump(), "application/json");
      return;
    }

    try {
      dispatch(method, body, [&](EmitCallbackData data) {
        res.set_content(data.dump(), "application/json");
      });
    } catch (const std::exception& exc) {
      SZ_LOG_ERROR("Message err: {}", exc.what());
    }

    // res.set_content("Hello World!", "application/json");
  };

  server_->Post(R"(^/db/(.+))", handler);

  server_->Get("/version", [&](const Request& req, Response& res) {
    json body = {
        {"version", GIT_DESCRIBE},
        {"quface_sdk_version", QuFaceSDK_VERSION},
        {"quface_io_sdk_version", QuFaceIOSDK_VERSION},
    };
    res.set_content(body.dump(), "application/json");
  });

  server_->Post("/config", [&](const Request& req, Response& res) {
    try {
      auto cfg = Config::get_instance();
      SZ_RETCODE ret;
      auto j = json::parse(req.body);
      ret = cfg->save_diff(j);
      if (ret != SZ_RETCODE_OK) {
        json data = {{"ok", false},
                     {"message", "save error " + std::to_string(ret)}};
        res.set_content(data.dump(), "application/json");
        return;
      }

      ret = cfg->reload();
      if (ret != SZ_RETCODE_OK) {
        json data = {{"ok", false},
                     {"message", "reload error " + std::to_string(ret)}};
        res.set_content(data.dump(), "application/json");
        return;
      }

      json data = {{"ok", true}, {"message", "ok"}};
      res.set_content(data.dump(), "application/json");
    } catch (const std::exception& exc) {
      SZ_LOG_ERROR("Message err: {}", exc.what());
      json data = {{"ok", false}, {"message", exc.what()}};
      res.set_content(data.dump(), "application/json");
    }
  });

  server_->Get("/config", [&](const Request& req, Response& res) {
    auto cfg = Config::get_all();

    json body(cfg);
    res.set_content(body.dump(), "application/json");
  });

  server_->Post("/config/-/reset", [&](const Request& req, Response& res) {
    auto cfg = Config::get_instance();

    SZ_RETCODE ret = cfg->reset();
    if (ret != SZ_RETCODE_OK) {
      json data = {{"ok", false},
                   {"message", "save error " + std::to_string(ret)}};
      res.set_content(data.dump(), "application/json");
      return;
    }

    json data = {{"ok", true}, {"message", "ok"}};
    res.set_content(data.dump(), "application/json");
  });

  server_->Get("/isp/exposure-info", [&](const Request& req, Response& res) {
    auto cam_type = CAMERA_BGR;
    if (req.has_param("cam")) {
      if (req.get_param_value("cam") == "bgr") {
        cam_type = CAMERA_BGR;
      } else {
        cam_type = CAMERA_NIR;
      }
    }

    ISPExposureInfo exp_info;
    SZ_RETCODE ret =
        Engine::instance()->isp_query_exposure_info(cam_type, &exp_info);
    if (ret != SZ_RETCODE_OK) {
      json body = {{"ok", false}, {"message", "get exp info failed"}};
      res.set_content(body.dump(), "application/json");
      return;
    }

    json body(exp_info);
    res.set_content(body.dump(), "application/json");
  });

  server_->Get("/isp/wb-info", [&](const Request& req, Response& res) {
    auto cam_type = CAMERA_BGR;
    if (req.has_param("cam")) {
      if (req.get_param_value("cam") == "bgr") {
        cam_type = CAMERA_BGR;
      } else {
        cam_type = CAMERA_NIR;
      }
    }

    ISPWBInfo wb_info;
    SZ_RETCODE ret = Engine::instance()->isp_query_wb_info(cam_type, &wb_info);
    if (ret != SZ_RETCODE_OK) {
      json body = {{"ok", false}, {"message", "get wb info failed"}};
      res.set_content(body.dump(), "application/json");
      return;
    }

    json body(wb_info);
    res.set_content(body.dump(), "application/json");
  });

  server_->Get("/isp/inner-state-info", [&](const Request& req, Response& res) {
    auto cam_type = CAMERA_BGR;
    if (req.has_param("cam")) {
      if (req.get_param_value("cam") == "bgr") {
        cam_type = CAMERA_BGR;
      } else {
        cam_type = CAMERA_NIR;
      }
    }

    ISPInnerStateInfo inner_state_info;
    SZ_RETCODE ret = Engine::instance()->isp_query_inner_state_info(
        cam_type, &inner_state_info);
    if (ret != SZ_RETCODE_OK) {
      json body = {{"ok", false}, {"message", "get inner state failed"}};
      res.set_content(body.dump(), "application/json");
      return;
    }

    json body(inner_state_info);
    res.set_content(body.dump(), "application/json");
  });

  server_->listen(host.c_str(), port);
}
