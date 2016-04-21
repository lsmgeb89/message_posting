#include "server.h"

int main(int argc, char *argv[]) {
  uint16_t port(0);
  if (argc != 2 || !IsNumber(argv[1])) {
    error_server << "Wrong Parameter" << std::endl;
    return 0;
  } else {
    port = static_cast<uint16_t>(std::stoul(argv[1]));
  }

  message_posting::server::Server message_server;
  message_server.Start(port);

  return 0;
}
