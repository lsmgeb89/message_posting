#ifndef MESSAGE_POSTING_CLIENT_H_
#define MESSAGE_POSTING_CLIENT_H_

#include <iostream>
#include <vector>

#include "socket_util.h"
#include "message_util.h"

namespace message_posting {

namespace client {

class Client {
 public:
  Client(void);
  void Connect(const std::string& remote_host,
               const uint16_t& port = 2016);
  void Login(void);
  void Communicate(void);
 private:
  std::string name_;
  utils::Socket socket_;
  const std::vector<std::string> menu_;
  utils::MessageUtil message_util_;

  void DisplayMenu(void);
  void DisplayName(const utils::RequestType& request_type);
  void SendMessage(const utils::RequestType& request_type);
  void GetMessage(void);
  void Exit(void);
};

} // namespace client

} // namespace message_posting

#endif // MESSAGE_POSTING_CLIENT_H_
