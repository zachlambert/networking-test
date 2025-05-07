#pragma once

#include <poll.h>
#include <string>
#include <vector>

class TcpServer {
public:
  TcpServer() : listener_fd(-1) {}
  ~TcpServer();

  bool init(const std::string &port);
  void run();

private:
  int listener_fd;
  std::vector<int> client_fds;
  std::vector<struct pollfd> poll_fds;
};
