#pragma once

#include <string>

class TcpClient {
public:
  TcpClient() : fd(-1) {}
  ~TcpClient();

  bool connect(const std::string &hostname, const std::string &port);
  void send(const std::string &message);

private:
  int fd;
};
