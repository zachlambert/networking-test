#include "tcp_client.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

static void *get_in_addr(struct sockaddr *addr) {
  if (addr->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)addr)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)addr)->sin6_addr);
}

TcpClient::~TcpClient() {
  if (fd != -1) {
    close(fd);
  }
}

bool TcpClient::connect(const std::string &hostname, const std::string &port) {
  struct addrinfo hints;
  struct addrinfo *servinfo;
  struct addrinfo *p;
  int rv;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM; // TCP

  {
    int rc = getaddrinfo(hostname.c_str(), port.c_str(), &hints, &servinfo);
    if (rc != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
      return false;
    }
  }

  for (struct addrinfo *iter = servinfo; iter; iter = iter->ai_next) {
    fd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
    if (fd == -1) {
      perror("client: socket");
      continue;
    }
    if (::connect(fd, iter->ai_addr, iter->ai_addrlen) == -1) {
      close(fd);
      fd = -1;
      perror("client: connect");
      continue;
    }

    // Success

    char ip_addr[INET6_ADDRSTRLEN];
    inet_ntop(iter->ai_family, get_in_addr((struct sockaddr *)iter->ai_addr),
              ip_addr, sizeof(ip_addr));
    printf("Connected to %s\n", ip_addr);

    break;
  }
  freeaddrinfo(servinfo);

  return (fd != -1);
}

void TcpClient::send(const std::string &message) {
  ::send(fd, message.data(), message.size() + 1, 0);
}
