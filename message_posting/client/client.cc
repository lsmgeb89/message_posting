#include <cstring>
#include <netdb.h>
#include <iostream>

#include "client.h"

namespace message_posting {

namespace client {

void Client::Start(const std::string& remote_host,
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

  socket_.Connect(reinterpret_cast<const sockaddr*>(&remote_address));
}

} // namespace client

} // namespace message_posting
