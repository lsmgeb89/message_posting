#ifndef MESSAGE_POSTING_SERVER_H_
#define MESSAGE_POSTING_SERVER_H_

#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>

#include "socket_util.h"
#include "message_util.h"

namespace message_posting {

namespace server {

class Server {
 public:
  Server(void)
    : message_util_(socket_) {
  }

  void Start(const uint16_t& port = 2016);

  void Serve(const int client_socket);

 private:
  utils::Socket socket_;
  utils::MessageUtil message_util_;
  std::mutex mutex_output_;
  std::mutex mutex_known_users_;
  std::mutex mutex_connected_users_;
  std::mutex mutex_message_database_;
  typedef std::unordered_map<std::string, uint8_t> KnownList;
  KnownList known_users_;
  std::unordered_map<std::string, std::string> connected_users_;
  std::unordered_map<std::string, std::vector<utils::TextMessage>> message_database_;
  static constexpr KnownList::size_type max_user = 100;
  static constexpr uint8_t max_message_count = 10;

  void DisplayName(const utils::RequestType& request_type,
                   utils::MessageUtil& client_message_util);

  void SendMessage(const char* time_str,
                   const std::string& client_name,
                   utils::MessageUtil& client_message_util,
                   const utils::RequestType& request_type,
                   const utils::TextMessage& text_msg,
                   const char* recipient = nullptr);

  void GetMessage(const std::string& client_name,
                  utils::MessageUtil& client_message_util);

  void Exit(const std::string& client_name);

  // operations of Known List
  inline void AddUserToKnownList(const std::string& client_name);

  inline bool IsUserInKnownList(const std::string& client_name);

  inline KnownList::size_type GetKnownListSize(void);

  inline void StepUserSendMessageCount(const std::string& client_name);

  inline bool IsUserExceedMessageCount(const std::string& client_name);

  // operations of Connected List
  inline void AddUserToConnectedList(const std::string& client_name);

  inline void RemoveUserFromConnectedList(const std::string& client_name);

  inline bool IsUserInConnectedList(const std::string& client_name);
};

} // namespace server

} // namespace message_posting

#endif // MESSAGE_POSTING_SERVER_H_
