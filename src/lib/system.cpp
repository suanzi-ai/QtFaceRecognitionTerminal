#include "system.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>

#include <nlohmann/json.hpp>

#include <quface/logger.hpp>

#include "static_config.hpp"

using json = nlohmann::json;

using namespace suanzi;

static void trim(std::string& s) {
  s.erase(0, s.find_first_not_of(" "));
  s.erase(s.find_last_not_of(" ") + 1);
  s.erase(s.find_last_not_of("\n") + 1);
}

SZ_RETCODE System::exec(const char* cmd, std::string& result) {
  std::array<char, 128> buffer;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
  if (!pipe) {
    SZ_LOG_ERROR("popen() failed!");
    return SZ_RETCODE_FAILED;
  }
  result = "";
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return SZ_RETCODE_OK;
}

SZ_RETCODE System::get_current_network(std::string& name, std::string& ip,
                                       std::string& mac_address) {
  std::string result;

  name = "none";
  ip = "none";
  mac_address = "none";

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

  trim(name);
  trim(ip);

  char cmd[100];
  sprintf(cmd, "cat /sys/class/net/%s/address", name.c_str());
  ret = exec(cmd, mac_address);
  if (ret != SZ_RETCODE_OK) {
    SZ_LOG_ERROR("/sys/class/net/{}/address not exists", name);
    return ret;
  }

  trim(mac_address);
  mac_address.erase(std::remove(mac_address.begin(), mac_address.end(), ':'),
                    mac_address.end());

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

SZ_RETCODE System::get_hostname(std::string& hostname) {
  SZ_RETCODE ret = exec(
      "cat /user/quface-app/etc/dynamic.yaml | grep \"  name: \"", hostname);
  if (SZ_RETCODE_OK != ret)
    hostname = "3S";
  else
    hostname = hostname.substr(8);

  return ret;
}

SZ_RETCODE System::get_serial_number(std::string& serial_number) {
  std::string name, ip, mac;
  System::get_current_network(name, ip, mac);

  SZ_RETCODE ret = exec("cat /etc/serial-number", serial_number);
  if (SZ_RETCODE_OK != ret) {
    serial_number = "20200602" + mac.substr(0, 2);
  }
  return ret;
}

SZ_RETCODE System::get_fw_version(std::string& fw_version) {
  fw_version = APP_VERSION;
  return SZ_RETCODE_OK;
}
