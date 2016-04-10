#include "client.h"

int main(int argc, char *argv[]) {
  message_posting::client::Client message_client;
  message_client.Start("127.0.0.1");

  return 0;
}
