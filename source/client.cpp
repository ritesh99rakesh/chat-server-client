#include "lib.hpp"

#include <arpa/inet.h>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <ostream>
#include <print>
#include <sys/_types/_fd_def.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    std::println(stderr, "Failed to create socket");
    return 1;
  }

  struct sockaddr_in serv_addr {};
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);

  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    std::print(stderr, "Invalid address");
    return 1;
  }

  if (connect(sock, reinterpret_cast<struct sockaddr *>(&serv_addr),
              sizeof(serv_addr)) < 0) {
    std::println(stderr, "Connection failed");
    return 1;
  }

  std::println("Connected to server");

  std::string username;
  std::print("Enter username: ");
  std::getline(std::cin, username);

  Message msg = Message{username, "LOGIN"};
  send(sock, &msg, sizeof(msg), 0);

  auto flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);

  while (true) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(STDIN_FILENO, &read_fds);
    FD_SET(sock, &read_fds);

    auto max_fd = std::max(STDIN_FILENO, sock);

    std::print(stderr, "You: ");

    if (select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr) < 0) {
      std::println(stderr, "Select error");
      continue;
    }

    if (FD_ISSET(STDIN_FILENO, &read_fds)) {
      std::string message;
      std::getline(std::cin, message);
      std::print("\033[A\033[2K\r");
      std::println("You: {}", message);
      std::println("------------");
      msg = Message{username, message};
      if (send(sock, &msg, sizeof(msg), 0) < 0) {
        std::println("Failed to send message");
        continue;
      }
    }

    if (FD_ISSET(sock, &read_fds)) {
      auto bytes_read = recv(sock, &msg, sizeof(msg), 0);
      if (bytes_read > 0) {
        std::print("\033[2K\r");
        std::println("{}: {}", msg.sender, msg.message);
        std::println("------------");
      } else if (bytes_read == 0) {
        std::println(stderr, "Server disconnected");
        break;
      } else if (errno != EWOULDBLOCK) {
        std::println(stderr, "Receive error");
        continue;
      }
    }
  }
  close(sock);
  return 0;
}
