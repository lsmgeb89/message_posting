#ifndef MESSAGE_POSTING_SERVER_H_
#define MESSAGE_POSTING_SERVER_H_

#include "socket_util.h"

namespace message_posting {

namespace server {

class Server {
 public:
  void Start(const uint16_t& port = 2016);
  void Serve(void);
 private:
  utils::Socket socket_;
};

} // namespace server

} // namespace message_posting

#endif // MESSAGE_POSTING_SERVER_H_
