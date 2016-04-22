#include "client.h"

int main(int argc, char *argv[]) {
  uint16_t port(0);
  if (argc != 3 || !IsNumber(argv[2])) {
    error_client << "Wrong Parameter!" << std::endl;
    return 0;
  } else {
    port = static_cast<uint16_t>(std::stoul(argv[2]));
  }

  try {
    message_posting::client::Client message_client;
    message_client.Connect(argv[1], port);
    message_client.Login();
    message_client.Communicate();
  } catch (...) {
    error_client << "Exit!\n";
  }
  return 0;
}
