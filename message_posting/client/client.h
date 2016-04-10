#ifndef MESSAGE_POSTING_CLIENT_H_
#define MESSAGE_POSTING_CLIENT_H_

#include "socket_util.h"

namespace message_posting {

namespace client {

class Client {
 public:
  void Start(const std::string& remote_host,
             const uint16_t& port = 2016);
 private:
  utils::Socket socket_;
};

} // namespace client

} // namespace message_posting

#endif // MESSAGE_POSTING_CLIENT_H_
