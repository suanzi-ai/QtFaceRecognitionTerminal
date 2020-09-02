#include "http_server.hpp"

#include "isp.h"
#include "static_config.hpp"

using namespace suanzi;

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
    int pipe = Config::get_camera(true).pipe;
    if (req.has_param("cam")) {
      if (req.get_param_value("cam") == "bgr") {
        pipe = Config::get_camera(true).pipe;
      } else {
        pipe = Config::get_camera(false).pipe;
      }
    }

    ISP_EXP_INFO_S exp_info;
    if (!Isp::getInstance()->query_exposure_info(pipe, &exp_info)) {
      json body = {{"ok", false}, {"message", "get exp info failed"}};
      res.set_content(body.dump(), "application/json");
      return;
    }

    json body;
    body["expTime"] = exp_info.u32ExpTime;
    body["shortExpTime"] = exp_info.u32ShortExpTime;
    body["medianExpTime"] = exp_info.u32MedianExpTime;
    body["longExpTime"] = exp_info.u32LongExpTime;
    body["aGain"] = exp_info.u32AGain;
    body["dGain"] = exp_info.u32DGain;
    body["ispDGain"] = exp_info.u32ISPDGain;
    body["exposure"] = exp_info.u32Exposure;
    body["exposureIsMAX"] = exp_info.bExposureIsMAX;
    body["hist1024Value"] = exp_info.au32AE_Hist1024Value;
    body["aveLum"] = exp_info.u8AveLum;
    body["linesPer500ms"] = exp_info.u32LinesPer500ms;
    body["pIrisFNO"] = exp_info.u32PirisFNO;
    body["fps"] = exp_info.u32Fps;
    body["iso"] = exp_info.u32ISO;
    body["isoCalibrate"] = exp_info.u32ISOCalibrate;
    body["refExpRatio"] = exp_info.u32RefExpRatio;
    body["firstStableTime"] = exp_info.u32FirstStableTime;
    res.set_content(body.dump(), "application/json");
  });

  server_->Get("/isp/wb-info", [&](const Request& req, Response& res) {
    int pipe = Config::get_camera(true).pipe;
    if (req.has_param("cam")) {
      if (req.get_param_value("cam") == "bgr") {
        pipe = Config::get_camera(true).pipe;
      } else {
        pipe = Config::get_camera(false).pipe;
      }
    }

    ISP_WB_INFO_S wb_info;
    if (!Isp::getInstance()->query_wb_info(pipe, &wb_info)) {
      json body = {{"ok", false}, {"message", "get wb info failed"}};
      res.set_content(body.dump(), "application/json");
      return;
    }

    json body;
    body["rgain"] = wb_info.u16Rgain;
    body["grgain"] = wb_info.u16Grgain;
    body["gbgain"] = wb_info.u16Gbgain;
    body["bgain"] = wb_info.u16Bgain;
    body["saturation"] = wb_info.u16Saturation;
    body["colorTemp"] = wb_info.u16ColorTemp;
    body["ccm"] = wb_info.au16CCM;
    body["ls0CT"] = wb_info.u16LS0CT;
    body["ls1CT"] = wb_info.u16LS1CT;
    body["ls0Area"] = wb_info.u16LS0Area;
    body["ls1Area"] = wb_info.u16LS1Area;
    body["multiDegree"] = wb_info.u8MultiDegree;
    body["activeShift"] = wb_info.u16ActiveShift;
    body["firstStableTime"] = wb_info.u32FirstStableTime;
    body["inOutStatus"] = wb_info.enInOutStatus;
    body["bv"] = wb_info.s16Bv;

    res.set_content(body.dump(), "application/json");
  });

  server_->Get("/isp/inner-state-info", [&](const Request& req, Response& res) {
    int pipe = Config::get_camera(true).pipe;
    if (req.has_param("cam")) {
      if (req.get_param_value("cam") == "bgr") {
        pipe = Config::get_camera(true).pipe;
      } else {
        pipe = Config::get_camera(false).pipe;
      }
    }

    ISP_INNER_STATE_INFO_S inner_state_info;
    if (!Isp::getInstance()->query_inner_state_info(pipe, &inner_state_info)) {
      json body = {{"ok", false}, {"message", "get inner state failed"}};
      res.set_content(body.dump(), "application/json");
      return;
    }

    json body;
    body["textureStr"] = inner_state_info.au16TextureStr;
    body["edgeStr"] = inner_state_info.au16EdgeStr;
    body["textureFreq"] = inner_state_info.u16TextureFreq;
    body["edgeFreq"] = inner_state_info.u16EdgeFreq;
    body["overShoot"] = inner_state_info.u8OverShoot;
    body["underShoot"] = inner_state_info.u8UnderShoot;
    body["shootSupStr"] = inner_state_info.u8ShootSupStr;
    body["nrLscRatio"] = inner_state_info.u8NrLscRatio;
    body["coarseStr"] = inner_state_info.au16CoarseStr;
    body["wdrFrameStr"] = inner_state_info.au8WDRFrameStr;
    body["chromaStr"] = inner_state_info.au8ChromaStr;
    body["fineStr"] = inner_state_info.u8FineStr;
    body["coringWgt"] = inner_state_info.u16CoringWgt;
    body["deHazeStrengthActual"] = inner_state_info.u16DeHazeStrengthActual;
    body["drcStrengthActual"] = inner_state_info.u16DrcStrengthActual;
    body["wdrExpRatioActual"] = inner_state_info.u32WDRExpRatioActual;
    body["wdrSwitchFinish"] = inner_state_info.bWDRSwitchFinish;
    body["resSwitchFinish"] = inner_state_info.bResSwitchFinish;
    body["blActual"] = inner_state_info.au16BLActual;

    res.set_content(body.dump(), "application/json");
  });

  server_->listen(host.c_str(), port);
}
