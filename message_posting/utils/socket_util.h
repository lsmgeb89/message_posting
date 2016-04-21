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

  Socket(const int& socket_descriptor)
    : socket_descriptor_(socket_descriptor) {
  }

  ~Socket(void) {
    if (-1 == ::close(socket_descriptor_)) {
      perror(LOG_ERROR_MODULE_SOCKET"[close]");
    }
  }

  Socket(const Socket&) = delete; // no copy
  Socket& operator=(const Socket&) = delete; // no assignment

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

  int Accept(sockaddr* address) {
    std::size_t address_len(sizeof(*address));
    int peer_socket = ::accept(socket_descriptor_,
                               address,
                               reinterpret_cast<socklen_t*>(&address_len));
    if (-1 == peer_socket) {
      perror(LOG_ERROR_MODULE_SOCKET"[accept]");
    }
    return peer_socket;
  }

  int Connect(const sockaddr* remote_address) {
    return ::connect(socket_descriptor_,
                     remote_address,
                     static_cast<socklen_t>(sizeof(*remote_address)));
  }

  ssize_t Read(void* buf, size_t count) const {
    return ::read(socket_descriptor_, buf, count);
  }

  ssize_t Write(const void* buf, size_t count) const {
    return ::write(socket_descriptor_, buf, count);
  }

 private:
  const int socket_descriptor_;
};

} // namespace utils

} // namespace message_posting

#endif // MESSAGE_POSTING_SOCKET_UTIL_H_
