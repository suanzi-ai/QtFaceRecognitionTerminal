#include "curl_util.hpp"

#include <curl/curl.h>

#include "logger.hpp"
#include "thread_pool.hpp"

static const char *user_agent = "QuFace";

static size_t writeCallback(void *contents, size_t size, size_t nmemb,
                            void *userp) {
  auto p = ((std::vector<SZ_BYTE> *)userp);
  p->insert(p->end(), (SZ_BYTE *)contents, (SZ_BYTE *)contents + size * nmemb);
  return size * nmemb;
}

SZ_RETCODE suanzi::download_to_buffer(
    const std::vector<std::string> &urls,
    std::map<std::string, std::vector<SZ_BYTE>> &bufferMap) {
  int threads_num = std::thread::hardware_concurrency() / 2 + 1;
  SZ_LOG_DEBUG("Download using {} workers", threads_num);
  ThreadPool pool(threads_num);

  for (auto &url : urls) {
    pool.enqueue([url, &bufferMap]() {
      std::vector<SZ_BYTE> buf;
      SZ_RETCODE ret = suanzi::download_to_buffer(url, buf);
      if (ret != SZ_RETCODE_OK) {
        return;
      }
      bufferMap.insert(std::make_pair(url, buf));
    });
  }

  return SZ_RETCODE_OK;
}

SZ_RETCODE suanzi::download_to_buffer(const std::string &url,
                                      std::vector<SZ_BYTE> &buffer) {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (!curl) {
    SZ_LOG_DEBUG("Curl init failed");
    return SZ_RETCODE_FAILED;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
  res = curl_easy_perform(curl);

  curl_easy_cleanup(curl);
  if (res != CURLE_OK) {
    SZ_LOG_WARN("Curl get failed {}, errno: {}", url, res);
    return SZ_RETCODE_FAILED;
  }
  SZ_LOG_TRACE("Curl get success {}", url);
  return SZ_RETCODE_OK;
}
