#include "logger.hpp"

#ifdef __ANDROID__
#include <spdlog/sinks/android_sink.h>
#else
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#endif

using namespace suanzi;

LoggerConfig Logger::config_ = {
#ifdef DEBUG
    "debug",  // log_level
#else
    "debug",  // log_level
#endif
    "stdout",           // log_std
    "logs/quface/log",  // log_file
    10 * 1024 * 1024,   // log_file_max_size
    5                   // log_file_max_count
};

LoggerPtr Logger::logger_ = NULL;

LoggerPtr Logger::create_logger() {
  LoggerPtr logger;

  spd::level::level_enum level = spd::level::level_enum::off;
  if (config_.log_level != "") {
    level = spd::level::from_str(config_.log_level);
  }

#ifdef __ANDROID__
  logger = spd::android_logger_mt("quface", "quface");
  logger->set_level(level);
#else
  spd::sink_ptr consoleLogger, rotatingLogger;

  if (config_.log_std == "stderr") {
    consoleLogger = std::make_shared<spd::sinks::stderr_color_sink_mt>();
    consoleLogger->set_level(level);
  } else {  // if (config_.log_std == "stdout")
    consoleLogger = std::make_shared<spd::sinks::stdout_color_sink_mt>();
    consoleLogger->set_level(level);
  }

  if (config_.log_file != "") {
    auto rotatingLogger = std::make_shared<spd::sinks::rotating_file_sink_mt>(
        config_.log_file, config_.log_file_max_size,
        config_.log_file_max_count);
    rotatingLogger->set_level(level);

    logger = std::make_shared<spd::logger>(
        "quface", spd::sinks_init_list({consoleLogger, rotatingLogger}));
  } else {
    logger = std::make_shared<spd::logger>(
        "quface", spd::sinks_init_list({consoleLogger}));
  }

  spd::details::registry::instance().flush_on(level);
  spd::details::registry::instance().flush_every(std::chrono::seconds(5));

  logger->set_level(level);
#endif

  return logger;
}

LoggerPtr Logger::get_logger() {
  if (!logger_) logger_ = create_logger();
  return logger_;
}
