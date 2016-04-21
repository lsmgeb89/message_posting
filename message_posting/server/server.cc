#include <cstring>
#include <ctime>
#include <iostream>
#include <sstream>
#include <thread>

#include <netinet/in.h>

#include "server.h"

namespace message_posting {

namespace server {

void Server::Start(const uint16_t& port) {
  char host[32];
  sockaddr_in local_address;

  std::memset(&local_address, 0, sizeof(local_address));
  local_address.sin_family = AF_INET;
  local_address.sin_addr.s_addr = INADDR_ANY;
  local_address.sin_port = htons(port);

  const sockaddr* address = reinterpret_cast<const sockaddr*>(&local_address);

  socket_.Bind(address);

  socket_.Listen();

  gethostname(host, sizeof(host));
  std::cout << "Server is running on " << host << ":" << port << std::endl;

  while (1) {
    sockaddr_in remote_address;
    int client_socket = socket_.Accept(reinterpret_cast<sockaddr*>(&remote_address));
    info_server << lock_with(mutex_output_) << "Start a new thread to serve client" << std::endl;
    std::thread server_thread(&Server::Serve, this, client_socket);
    server_thread.detach();
  }
}

void Server::Serve(const int client_socket_descriptor) {
  bool exit(false);
  char time_str[128];
  std::time_t t;

  utils::Socket client_socket(client_socket_descriptor);
  utils::MessageUtil client_message_util(client_socket);

  client_message_util.Read();
  const std::string client_name(client_message_util.GetClientName());

  std::memset(time_str, 0, 128);
  t = std::time(nullptr);
  std::strftime(time_str, sizeof(time_str), "%D, %R %p, ", std::localtime(&t));
  if (IsUserInKnownList(client_name)) {
    std::cout << lock_with(mutex_output_) << time_str
              << "Connection by known user " << client_name << std::endl;
  } else {
    std::cout << lock_with(mutex_output_) << time_str
              << "Connection by unknown user " << client_name << std::endl;
  }

  AddUserToConnectedList(client_name);
  AddUserToKnownList(client_name);

  do {
    client_message_util.Read();
    const utils::RequestType request_type(client_message_util.GetRequestType());

    std::memset(time_str, 0, 128);
    t = std::time(nullptr);
    std::strftime(time_str, sizeof(time_str), "%D, %R %p, ", std::localtime(&t));

#define SERVER_OUT std::cout << lock_with(mutex_output_) << time_str << client_name

    switch (request_type) {
      case utils::DisplayKnownUsersNames:
        SERVER_OUT << " displays all known users." << std::endl;
      case utils::DisplayConnectedUsersNames:
        SERVER_OUT << " displays all connected users." << std::endl;
        DisplayName(request_type, client_message_util);
        break;
      case utils::SendMessage2User:
        SERVER_OUT << " posts a message for " << client_message_util.GetRecipient()
                   << "." << std::endl;
        SendMessage(request_type,
                    client_message_util.GetTextMessage(),
                    client_message_util.GetRecipient());
        break;
      case utils::SendMessage2ConnectedUsers:
      case utils::SendMessage2KnownUsers:
          SendMessage(request_type,
                      client_message_util.GetTextMessage());
        break;
      case utils::GetMessages:
        SERVER_OUT << " gets messages." << std::endl;
        GetMessage(client_name, client_message_util);
        break;
      case utils::Exit:
        SERVER_OUT << " exits." << std::endl;
        Exit(client_name);
        exit = true;
        break;
      default:
        break;
    }
  } while(!exit);

#undef SERVER_OUT
}

void Server::DisplayName(const utils::RequestType &request_type,
                         utils::MessageUtil& client_message_util) {
  std::stringstream str_stream;
  utils::PropertyList list;

  if (utils::DisplayKnownUsersNames == request_type) {
    std::lock_guard<std::mutex> guard(mutex_known_users_);
    for (auto it = known_users_.cbegin(); it != known_users_.cend(); ++it) {
      str_stream << std::distance(known_users_.cbegin(), it) + 1 << ". " << it->first << std::endl;
    }
  } else if (utils::DisplayConnectedUsersNames == request_type) {
    std::lock_guard<std::mutex> guard(mutex_connected_users_);
    for (auto it = connected_users_.cbegin(); it != connected_users_.cend(); ++it) {
      str_stream << std::distance(connected_users_.cbegin(), it) + 1 << ". " << it->first << std::endl;
    }
  } else {
    error_server << lock_with(mutex_output_) << "Wrong request type for DisplayName" << std::endl;
    return;
  }

  list = {{"z", str_stream.str()}};
  client_message_util.SetResponse(list);
  client_message_util.Write();
}

void Server::SendMessage(const utils::RequestType& request_type,
                         const utils::TextMessage& text_msg,
                         const char* recipient) {
  std::lock_guard<std::mutex> guard(mutex_message_database_);
  if (utils::SendMessage2User == request_type && recipient) {
    // a message sent to an unknown user makes them known
    AddUserToKnownList(recipient);
    message_database_[recipient].push_back(text_msg);
  } else if (utils::SendMessage2ConnectedUsers == request_type) {
    std::lock_guard<std::mutex> guard(mutex_connected_users_);
    for (auto it : connected_users_) {
      // exclude sending message to himself
      if (std::strcmp(text_msg.sender_, it.first.c_str())) {
        message_database_[it.first].push_back(text_msg);
      }
    }
  } else if (utils::SendMessage2KnownUsers == request_type) {
    std::lock_guard<std::mutex> guard(mutex_known_users_);
    for (auto it : known_users_) {
      // exclude sending message to himself
      if (std::strcmp(text_msg.sender_, it.first.c_str())) {
        message_database_[it.first].push_back(text_msg);
      }
    }
  } else {
    error_server << lock_with(mutex_output_)
                 << "Wrong request type for SendMessage" << std::endl;
  }
}

void Server::GetMessage(const std::string client_name,
                        utils::MessageUtil& client_message_util) {
  std::stringstream str_stream;
  utils::PropertyList list;
  char time_str[128];

  {
    std::lock_guard<std::mutex> guard(mutex_message_database_);
    std::vector<utils::TextMessage>& message_list = message_database_[client_name];
    for (auto it = message_list.cbegin(); it != message_list.cend(); ++it) {
      str_stream << std::distance(message_list.cbegin(), it) + 1 << ". ";
      str_stream << "From " << it->sender_ << ", ";
      std::memset(time_str, 0, 128);
      if (std::strftime(time_str, sizeof(time_str), "%D %R %p", std::localtime(&it->time_))) {
        str_stream << time_str << ", ";
      }
      str_stream << it->words_ << std::endl;
    }
  }

  list = {{"z", str_stream.str()}};
  client_message_util.SetResponse(list);
  client_message_util.Write();
}

void Server::Exit(const std::string& client_name) {
  RemoveUserFromConnectedList(client_name);
}

void Server::AddUserToKnownList(const std::string &client_name) {
  std::lock_guard<std::mutex> guard(mutex_known_users_);
  known_users_[client_name] = client_name;
  info_server << lock_with(mutex_output_)
              << "Add client " << client_name << " to known list." << std::endl;
}

bool Server::IsUserInKnownList(const std::string &client_name) {
  std::lock_guard<std::mutex> guard(mutex_known_users_);
  auto it = known_users_.find(client_name);
  return (it != known_users_.end());
}

void Server::AddUserToConnectedList(const std::string &client_name) {
  std::lock_guard<std::mutex> guard(mutex_connected_users_);
  connected_users_[client_name] = client_name;
  info_server << lock_with(mutex_output_)
              << "Add client " << client_name << " to connected list." << std::endl;
}

void Server::RemoveUserFromConnectedList(const std::string &client_name) {
  std::lock_guard<std::mutex> guard(mutex_connected_users_);
  auto removed_count = connected_users_.erase(client_name);
  if (removed_count) {
    info_server << lock_with(mutex_output_)
                << "Remove client " << client_name << " from connected list." << std::endl;
  }
}

} // namespace server

} // namespace message_posting
