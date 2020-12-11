#pragma once

#include <quface/common.hpp>

namespace suanzi {
class System {
 public:
  static SZ_RETCODE exec(const char* cmd, std::string& result);
  static SZ_RETCODE get_current_network(std::string& name, std::string& ip,
                                        std::string& mac_address);
  static SZ_RETCODE get_release_version(std::string& version);
  static SZ_RETCODE get_hostname(std::string& hostname);
  static SZ_RETCODE get_serial_number(std::string& serial_number);
};

}  // namespace suanzi
