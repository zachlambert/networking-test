#include "render.hpp"
#include "tcp_client.hpp"
#include <iostream>

int main() {
  const std::string host = "localhost";
  const std::string port = "3490";

  Grid grid(10);
  grid(0, 0) = Pixel::Red;

  TcpClient client;
  client.connect(host, port);
  while (true) {
    std::string line;
    std::getline(std::cin, line);
    if (line == "quit") {
      break;
    }
    client.send(line);
  }
}
