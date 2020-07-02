#ifndef __CURL_UTIL_HPP__
#define __CURL_UTIL_HPP__

#include <map>
#include <string>
#include <vector>

#include <quface/common.hpp>

namespace suanzi {
SZ_RETCODE download_to_buffer(
    const std::vector<std::string> &urls,
    std::map<std::string, std::vector<SZ_BYTE>> &bufferMap);
SZ_RETCODE download_to_buffer(const std::string &url,
                              std::vector<SZ_BYTE> &buffer);
};  // namespace suanzi

#endif
