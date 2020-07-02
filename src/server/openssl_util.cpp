#include "openssl_util.hpp"

#include <assert.h>
#include <openssl/bio.h>
#include <openssl/evp.h>

#include <cstring>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "openssl-compat.h"

namespace {
struct BIOFreeAll {
  void operator()(BIO* p) { BIO_free_all(p); }
};
}  // namespace

std::vector<SZ_BYTE> suanzi::server::base64_decode(const char* encoded) {
  std::unique_ptr<BIO, BIOFreeAll> b64(BIO_new(BIO_f_base64()));
  BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
  BIO* source = BIO_new_mem_buf(encoded, -1);  // read-only source
  BIO_push(b64.get(), source);
  const int maxlen = strlen(encoded) / 4 * 3 + 1;
  std::vector<SZ_BYTE> decoded(maxlen);
  const int len = BIO_read(b64.get(), decoded.data(), maxlen);
  decoded.resize(len);
  return decoded;
}

std::string suanzi::server::base64_encode(const std::vector<SZ_BYTE>& binary) {
  std::unique_ptr<BIO, BIOFreeAll> b64(BIO_new(BIO_f_base64()));
  BIO_set_flags(b64.get(), BIO_FLAGS_BASE64_NO_NL);
  BIO* sink = BIO_new(BIO_s_mem());
  BIO_push(b64.get(), sink);
  BIO_write(b64.get(), binary.data(), binary.size());
  BIO_flush(b64.get());
  const char* encoded;
  const long len = BIO_get_mem_data(sink, &encoded);
  return std::string(encoded, len);
}

static const char hex_digits[] = "0123456789ABCDEF";

std::string suanzi::server::hex_encode(const unsigned char* in, size_t length) {
  std::string out;
  out.reserve(length * 2);
  for (int i = 0; i < length; i++) {
    SZ_BYTE c = in[i];
    out.push_back(hex_digits[c >> 4]);
    out.push_back(hex_digits[c & 15]);
  }
  return out;
}

std::string suanzi::server::random_string(std::string::size_type length) {
  static auto& chrs =
      "0123456789"
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  thread_local static std::mt19937 rg{std::random_device{}()};
  thread_local static std::uniform_int_distribution<std::string::size_type>
      pick(0, sizeof(chrs) - 2);

  std::string s;

  s.reserve(length);

  while (length--) s += chrs[pick(rg)];

  return s;
}
