#ifndef MESSAGE_POSTING_SOCKET_UTIL_H_
#define MESSAGE_POSTING_SOCKET_UTIL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include "log_util.h"

namespace message_posting {

namespace utils {

class Socket {
 public:
  Socket(void)
    : socket_descriptor_(::socket(AF_INET, SOCK_STREAM, 0)) {
    if (-1 == socket_descriptor_) {
      perror(LOG_ERROR_MODULE_SOCKET"[socket]");
      // TODO: more error handling
    }
  }

  ~Socket(void) {
    if (-1 == ::close(socket_descriptor_)) {
      perror(LOG_ERROR_MODULE_SOCKET"[close]");
    }
  }

  void Bind(const sockaddr* address) {
    int ret = ::bind(socket_descriptor_,
                     address,
                     static_cast<socklen_t>(sizeof(*address)));
    if (-1 == ret) {
      std::cerr << errno;
      perror(LOG_ERROR_MODULE_SOCKET"[bind]");
    }
  }

  void Listen(void) {
    int ret = ::listen(socket_descriptor_, SOMAXCONN);
    // TODO: adjust the length
    if (-1 == ret) {
      perror(LOG_ERROR_MODULE_SOCKET"[listen]");
    }
  }

  void Accept(sockaddr* address) {
    std::size_t address_len(sizeof(*address));
    int ret = ::accept(socket_descriptor_,
                       address,
                       reinterpret_cast<socklen_t*>(&address_len));
    if (-1 == ret) {
      perror(LOG_ERROR_MODULE_SOCKET"[accept4]");
    }
  }

  int Connect(const sockaddr* remote_address) {
    return ::connect(socket_descriptor_,
                     remote_address,
                     static_cast<socklen_t>(sizeof(*remote_address)));
  }

  ssize_t Read(void* buf, size_t count) {
    return ::read(socket_descriptor_, buf, count);
  }

  ssize_t Write(const void* buf, size_t count) {
    return ::write(socket_descriptor_, buf, count);
  }

 private:
  int socket_descriptor_;
};

} // namespace utils

} // namespace message_posting

#endif // MESSAGE_POSTING_SOCKET_UTIL_H_
