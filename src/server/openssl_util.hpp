#ifndef _OPENSSL_UTIL_HPP_
#define _OPENSSL_UTIL_HPP_

#include <quface/common.hpp>
#include <string>
#include <vector>

namespace suanzi {
namespace server {
std::string random_string(std::string::size_type length);

std::vector<SZ_BYTE> base64_decode(const char* encoded);
std::string base64_encode(const std::vector<SZ_BYTE>& binary);

std::string hex_encode(const unsigned char* in, size_t length);
};  // namespace server
};  // namespace suanzi

#endif /* _OPENSSL_UTIL_HPP_ */
