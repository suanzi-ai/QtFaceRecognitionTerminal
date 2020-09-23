#pragma once

#include <quface/common.hpp>

namespace suanzi {
class System {
 public:
  static SZ_RETCODE exec(const char* cmd, std::string& result);
  static SZ_RETCODE get_current_network(std::string& name, std::string& ip);
  static SZ_RETCODE get_release_version(std::string& version);
};

}  // namespace suanzi
