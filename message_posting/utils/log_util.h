#ifndef MESSAGE_POSTING_LOG_UTIL_H_
#define MESSAGE_POSTING_LOG_UTIL_H_

#include <algorithm>
#include <iomanip>
#include <mutex>

#ifndef NDEBUG
#define debug_clog std::clog

#define MODULE_SERVER "[server] "
#define MODULE_CLIENT "[client] "
#define MODULE_SOCKET "[socket] "
#define MODULE_MESSAGE "[message] "

#define LOG_ERROR "[error]"
#define LOG_INFO "[info]"
#else
class NullBuffer : public std::streambuf {
public:
  int overflow(int c) { return c; }
};

class NullStream : public std::ostream {
  public:
    NullStream() : std::ostream(&m_sb) {}
  private:
    NullBuffer m_sb;
};
static NullStream null_stream;

#define debug_clog null_stream

#define MODULE_SERVER ""
#define MODULE_CLIENT ""
#define MODULE_SOCKET ""
#define MODULE_MESSAGE ""

#define LOG_ERROR ""
#define LOG_INFO ""
#endif

#define info_server debug_clog << LOG_INFO << MODULE_SERVER
#define info_client debug_clog << LOG_INFO << MODULE_CLIENT
#define info_socket debug_clog << LOG_INFO << MODULE_SOCKET
#define info_message debug_clog << LOG_INFO << MODULE_MESSAGE

#define error_server std::cerr << LOG_ERROR << MODULE_SERVER
#define error_client std::cerr << LOG_ERROR << MODULE_CLIENT
#define error_socket std::cerr << LOG_ERROR << MODULE_SOCKET
#define error_message std::cerr << LOG_ERROR << MODULE_MESSAGE

#define LOG_ERROR_MODULE_CLIENT LOG_ERROR MODULE_CLIENT
#define LOG_ERROR_MODULE_SOCKET LOG_ERROR MODULE_SOCKET
#define LOG_ERROR_MODULE_MESSAGE LOG_ERROR MODULE_MESSAGE

inline std::ostream& operator<<(std::ostream& out,
                                const std::lock_guard<std::mutex> &) {
  return out;
}

template <typename T> inline std::lock_guard<T> lock_with(T &mutex) {
  mutex.lock();
  return { mutex, std::adopt_lock };
}

inline bool IsNumber(const std::string& str) {
  return (!str.empty() &&
          std::find_if(str.begin(), str.end(),
                       [](char c) { return !std::isdigit(c); }) == str.end());
}

#endif // MESSAGE_POSTING_LOG_UTIL_H_
