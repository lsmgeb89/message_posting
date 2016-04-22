#include <cstring>
#include <ctime>
#include <netdb.h>
#include <iostream>

#include "client.h"

namespace message_posting {

namespace client {

Client::Client(void)
  : menu_{"Display the names of all known users",
          "Display the names of all currently connected users",
          "Send a text message to a particular user",
          "Send a text message to all currently connected users",
          "Send a text message to all known users",
          "Get my messages",
          "Exit"},
    message_util_(socket_) {
}

void Client::Connect(const std::string& remote_host,
                     const uint16_t &port) {
  hostent* remote_host_info = gethostbyname(remote_host.c_str());
  if (!remote_host_info) {
    perror(LOG_ERROR_MODULE_CLIENT"[gethostbyname]");
  }

  sockaddr_in remote_address;
  std::memset(&remote_address, 0, sizeof(remote_address));
  remote_address.sin_family = AF_INET;
  remote_address.sin_port = htons(port);
  remote_address.sin_addr = *reinterpret_cast<in_addr*>(remote_host_info->h_addr);

  std::cout << "Connecting to " << remote_host << ":" << port << std::endl;

  // TODO: handle error
  socket_.Connect(reinterpret_cast<const sockaddr*>(&remote_address));
}

void Client::Login(void) {
  std::cout << "Enter your name: ";
  std::cin >> name_;

  utils::PropertyList list{{"n", name_}};
  message_util_.SetRequest(utils::Login, list);
  message_util_.Write();

  message_util_.Read();
  if (utils::R_FAIL == message_util_.GetRetCode()) {
    error_client << message_util_.GetResponse() << std::endl;
    throw std::logic_error("");
  } else {
    info_client << message_util_.GetResponse() << std::endl;
  }
}

void Client::Communicate(void) {
  bool exit = false;

  do {
    DisplayMenu();

    utils::RequestType choice;
    std::cout << "Enter your choice: ";
    try {
      std::cin >> choice;
    } catch (const std::invalid_argument& e_arg) {
      error_client << "Invalid number! Please try again!";
    } catch (const std::out_of_range& e_range) {
      error_client << e_range.what();
    } catch (const std::system_error& e_sys) {
      error_client << "Unrecoverable I/O error!" << std::endl;
      throw e_sys;
    }
    // discard the Enter
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cout << std::endl;

    switch (choice) {
      case utils::DisplayKnownUsersNames:
      case utils::DisplayConnectedUsersNames:
        DisplayName(choice);
        break;
      case utils::SendMessage2User:
      case utils::SendMessage2ConnectedUsers:
      case utils::SendMessage2KnownUsers:
        SendMessage(choice);
        break;
      case utils::GetMessages:
        GetMessage();
        break;
      case utils::Exit:
        Exit();
        exit = true;
        break;
      default:
        break;
    }
  } while(!exit);
}

void Client::DisplayMenu(void) {
  std::stringstream output;
  output << std::endl;
  for (auto it = menu_.cbegin(); it != menu_.cend(); ++it) {
    output << std::distance(menu_.cbegin(), it) + 1 << ". " << *it << ".\n";
  }
  std::cout << output.str();
}

void Client::DisplayName(const utils::RequestType& request_type) {
  message_util_.SetRequest(request_type);
  message_util_.Write();
  message_util_.Read();
  if (utils::DisplayKnownUsersNames == request_type) {
    std::cout << "Known users:" << std::endl;
  } else if (utils::DisplayConnectedUsersNames == request_type) {
    std::cout << "Currently connected users:" << std::endl;
  }
  std::cout << message_util_.GetResponse();
}

void Client::SendMessage(const utils::RequestType& request_type) {
  std::string recipient;
  std::string text_msg;
  utils::PropertyList list;

  if (utils::SendMessage2User == request_type) {
    std::cout << "Enter recipient's name: ";
    std::getline(std::cin, recipient);
  }
  std::cout << "Enter a message: ";
  std::getline(std::cin, text_msg);

  std::time_t current_time = std::time(nullptr);
  list = {{"s", name_}, {"t", std::to_string(current_time)}, {"m", text_msg}};

  if (utils::SendMessage2User == request_type) {
    list.push_back({"r", recipient});
  }

  message_util_.SetRequest(request_type, list);
  message_util_.Write();

  if (utils::SendMessage2User == request_type) {
    std::cout << "\nMessage posted to " << recipient << std::endl;
  } else if (utils::SendMessage2ConnectedUsers == request_type) {
    std::cout << "\nMessage posted to all currently connected users." << std::endl;
  } else if (utils::SendMessage2KnownUsers == request_type) {
    std::cout << "\nMessage posted to all known users." << std::endl;
  }
}

void Client::GetMessage(void) {
  message_util_.SetRequest(utils::GetMessages);
  message_util_.Write();
  message_util_.Read();
  std::cout << "Your messages:" << std::endl << message_util_.GetResponse();
}

void Client::Exit(void) {
  message_util_.SetRequest(utils::Exit);
  message_util_.Write();
}

} // namespace client

} // namespace message_posting
