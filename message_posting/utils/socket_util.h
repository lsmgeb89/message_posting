#ifndef MESSAGE_POSTING_SOCKET_UTIL_H_
#define MESSAGE_POSTING_SOCKET_UTIL_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <iostream>
#include <system_error>

#include "log_util.h"

namespace message_posting {

namespace utils {

class Socket {
 public:
  Socket(void)
    : socket_descriptor_(::socket(AF_INET, SOCK_STREAM, 0)) {
    if (-1 == socket_descriptor_) {
      perror(LOG_ERROR_MODULE_SOCKET"[socket]");
      throw std::system_error(errno, std::system_category(), "[socket]");
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
      perror(LOG_ERROR_MODULE_SOCKET"[bind]");
      throw std::system_error(errno, std::system_category(), "[bind]");
    }
  }

  void Listen(void) {
    int ret = ::listen(socket_descriptor_, SOMAXCONN);
    if (-1 == ret) {
      perror(LOG_ERROR_MODULE_SOCKET"[listen]");
      throw std::system_error(errno, std::system_category(), "[listen]");
    }
  }

  int Accept(sockaddr* address) {
    std::size_t address_len(sizeof(*address));
    int peer_socket = ::accept(socket_descriptor_,
                               address,
                               reinterpret_cast<socklen_t*>(&address_len));
    if (-1 == peer_socket) {
      perror(LOG_ERROR_MODULE_SOCKET"[accept]");
      throw std::system_error(errno, std::system_category(), "[accept]");
    }
    return peer_socket;
  }

  int Connect(const sockaddr* remote_address) {
    int res = ::connect(socket_descriptor_,
                        remote_address,
                        static_cast<socklen_t>(sizeof(*remote_address)));
    if (-1 == res) {
      perror(LOG_ERROR_MODULE_SOCKET"[connect]");
      throw std::system_error(errno, std::system_category(), "[connect]");
    }
    return res;
  }

  ssize_t Read(void* buf, size_t count) const {
    int res = ::read(socket_descriptor_, buf, count);
    if (-1 == res) {
      perror(LOG_ERROR_MODULE_SOCKET"[read]");
      throw std::system_error(errno, std::system_category(), "[read]");
    }
    return res;
  }

  ssize_t Write(const void* buf, size_t count) const {
    int res = ::write(socket_descriptor_, buf, count);
    if (-1 == res) {
      perror(LOG_ERROR_MODULE_SOCKET"[write]");
      throw std::system_error(errno, std::system_category(), "[write]");
    }
    return res;
  }

 private:
  const int socket_descriptor_;
};

} // namespace utils

} // namespace message_posting

#endif // MESSAGE_POSTING_SOCKET_UTIL_H_
