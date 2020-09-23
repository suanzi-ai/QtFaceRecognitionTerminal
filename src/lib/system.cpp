#include "system.hpp"

#include <cstdio>
#include <fstream>
#include <nlohmann/json.hpp>
#include <quface/logger.hpp>

using json = nlohmann::json;

using namespace suanzi;

SZ_RETCODE System::exec(const char* cmd, std::string& result) {
  std::array<char, 128> buffer;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    SZ_LOG_ERROR("popen() failed!");
    return SZ_RETCODE_FAILED;
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return SZ_RETCODE_OK;
}

SZ_RETCODE System::get_current_network(std::string& name, std::string& ip) {
  std::string result;

  name = "none";
  ip = "none";

  // 8.8.8.8 via 192.168.2.1 dev eth0  src 192.168.2.12
  SZ_RETCODE ret = exec("ip route get 8.8.8.8", result);
  if (ret != SZ_RETCODE_OK) {
    return ret;
  }

  if (result.find("unreachable") != std::string::npos) {
    SZ_LOG_ERROR("get_current_network failed {}", result);
    return SZ_RETCODE_FAILED;
  }

  size_t dev_idx = result.find("dev");
  size_t src_idx = result.find("src");
  if (dev_idx == std::string::npos || src_idx == std::string::npos) {
    SZ_LOG_ERROR("Parse result error {}", result);
    return SZ_RETCODE_FAILED;
  }

  dev_idx += 4;
  src_idx += 4;

  name = result.substr(dev_idx, src_idx - dev_idx - 6 + 1);
  ip = result.substr(src_idx, result.length() - src_idx);

  return SZ_RETCODE_OK;
}

SZ_RETCODE System::get_release_version(std::string& version) {
  version = "unknown";

  std::ifstream i("/user/quface-app/etc/app-release.json");
  if (!i.is_open()) {
    return SZ_RETCODE_FAILED;
  }

  json data;
  i >> data;
  if (!data.contains("version")) {
    return SZ_RETCODE_FAILED;
  }

  version = data["version"];
  return SZ_RETCODE_OK;
}
