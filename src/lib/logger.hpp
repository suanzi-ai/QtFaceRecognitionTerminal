#pragma once

#include <quface/common.hpp>

#include <spdlog/spdlog.h>

#define SZ_LOG_TRACE(...) \
  SPDLOG_LOGGER_TRACE(suanzi::Logger::get_logger(), __VA_ARGS__)
#define SZ_LOG_DEBUG(...) \
  SPDLOG_LOGGER_DEBUG(suanzi::Logger::get_logger(), __VA_ARGS__)
#define SZ_LOG_INFO(...) \
  SPDLOG_LOGGER_INFO(suanzi::Logger::get_logger(), __VA_ARGS__)
#define SZ_LOG_WARN(...) \
  SPDLOG_LOGGER_WARN(suanzi::Logger::get_logger(), __VA_ARGS__)
#define SZ_LOG_ERROR(...) \
  SPDLOG_LOGGER_ERROR(suanzi::Logger::get_logger(), __VA_ARGS__)
#define SZ_LOG_CRITICAL(...) \
  SPDLOG_LOGGER_CRITICAL(suanzi::Logger::get_logger(), __VA_ARGS__)

#define SZ_LOG_UNTESTED(tag) \
  SZ_LOG_WARN("{} is untested, use with cautions", tag)

namespace suanzi {

namespace spd = spdlog;

using LoggerPtr = std::shared_ptr<spd::logger>;

typedef struct {
  std::string log_level;
  std::string log_std;
  std::string log_file;
  size_t log_file_max_size;
  size_t log_file_max_count;
} LoggerConfig;

class Logger {
 public:
  static LoggerPtr get_logger();

 private:
  Logger();
  ~Logger();

  static LoggerPtr create_logger();

  static LoggerPtr logger_;
  static LoggerConfig config_;
};

};  // namespace suanzi
