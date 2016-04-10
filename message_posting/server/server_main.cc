#include "server.h"

int main(int argc, char *argv[]) {
  message_posting::server::Server message_server;
  message_server.Start();

  return 0;
}
