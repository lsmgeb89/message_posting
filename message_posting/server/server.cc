#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <thread>

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
    socket_.Accept(reinterpret_cast<sockaddr*>(&remote_address));
    std::thread server_thread(&Server::Serve, this);
    server_thread.detach();
  }
}

void Server::Serve(void) {
  std::cout << "Serve thread" << std::endl;
}

} // namespace server

} // namespace message_posting
