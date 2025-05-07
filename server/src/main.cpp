#include "tcp_server.hpp"

int main(void) {
  const std::string port = "3490";

  TcpServer server;
  if (!server.init(port)) {
    return 1;
  }
  printf("Server started on port %s\n", port.c_str());
  server.run();
}
