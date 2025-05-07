#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

int get_server_sockfd(const char *port) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; // Host IP

  struct addrinfo *servinfo;
  {
    int rc = getaddrinfo(NULL, port, &hints, &servinfo);
    if (rc != 0) {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rc));
      return 1;
    }
  }

  int sockfd = -1;
  for (struct addrinfo *iter = servinfo; iter; iter = iter->ai_next) {
    sockfd = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
    if (sockfd == -1) {
      perror("server: socket");
      continue;
    }

    int optval = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) ==
        -1) {
      close(sockfd);
      sockfd = -1;
      perror("setsockopt");
      continue;
    }

    if (bind(sockfd, iter->ai_addr, iter->ai_addrlen) == -1) {
      close(sockfd);
      sockfd = -1;
      perror("server: bind");
      continue;
    }
  }
  freeaddrinfo(servinfo);

  return sockfd;
}

void *get_in_addr(struct sockaddr *addr) {
  if (addr->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)addr)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)addr)->sin6_addr);
}

class Server {
public:
  Server() : listener_fd(-1) {}

  bool init(const std::string &port) {
    listener_fd = get_server_sockfd(port.c_str());
    if (listener_fd == -1) {
      return false;
    }

    const int backlog = 10;
    if (listen(listener_fd, backlog) == 1) {
      perror("listen");
      close(listener_fd);
      listener_fd = -1;
      return false;
    }

    poll_fds.emplace_back();
    poll_fds[0].fd = listener_fd;
    poll_fds[0].events = POLLIN;
    poll_fds[0].revents = 0;

    return true;
  }

  void run() {
    while (true) {
      int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
      if (poll_count == -1) {
        perror("poll");
        return;
      }

      std::vector<int> client_fds_to_add;
      std::vector<int> client_fds_to_remove;

      for (auto &poll_fd : poll_fds) {
        if (!(poll_fd.revents & (POLLIN | POLLHUP))) {
          continue;
        }
        if (poll_fd.fd == listener_fd) {
          // New connection
          struct sockaddr_storage client_addr;
          socklen_t client_addr_len = sizeof(client_addr);
          int client_fd = accept(listener_fd, (struct sockaddr *)&client_addr,
                                 &client_addr_len);
          if (client_fd == -1) {
            perror("accept");
            return;
          }

          char client_ip[INET6_ADDRSTRLEN];
          inet_ntop(client_addr.ss_family,
                    get_in_addr((struct sockaddr *)&client_addr), client_ip,
                    sizeof(client_ip));

          client_fds_to_add.push_back(client_fd);
          printf("New connection: from %s on socket %d", client_ip, client_fd);
          continue;
        }

        // Otherwise, handle a client message
        char buffer[128];
        int nbytes = recv(poll_fd.fd, buffer, sizeof(buffer), 0);
        if (nbytes <= 0) {
          client_fds_to_remove.push_back(poll_fd.fd);
          continue;
        }

        std::size_t message_len = strnlen(buffer, nbytes);
        if (message_len >= nbytes) {
          fprintf(stderr, "Invalid data received\n");
        }
        printf("Received message:\n%s\n", buffer);
      }

      for (int client_fd : client_fds_to_add) {
        printf("Adding connection %zu\n", client_fds.size());
        client_fds.push_back(client_fd);
        poll_fds.emplace_back();
        poll_fds.back().fd = client_fd;
        poll_fds.back().events = POLLIN;
        poll_fds.back().revents = 0;
      }
      for (int client_fd : client_fds_to_remove) {
        std::size_t index = 0;
        while (index < poll_fds.size()) {
          if (client_fds[index] == client_fd) {
            break;
          }
          index++;
        }
        printf("Removing connection %zu\n", index);
        client_fds.erase(client_fds.begin() + index);
        poll_fds.erase(poll_fds.begin() + index);
      }
    }
  }

  ~Server() {
    if (listener_fd) {
      close(listener_fd);
    }
    for (int client_fd : client_fds) {
      if (client_fd) {
        close(client_fd);
      }
    }
  }

private:
  int listener_fd;
  std::vector<int> client_fds;
  std::vector<struct pollfd> poll_fds;
};

int main(void) {
  const std::string port = "3490";

  Server server;
  if (!server.init(port)) {
    return 1;
  }
  printf("Server started on port %s\n", port.c_str());
  server.run();
}
