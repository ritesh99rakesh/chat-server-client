#include "lib.hpp"

#include <cstdio>
#include <netinet/in.h>
#include <print>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class ClientSocket {
public:
  ClientSocket(std::string name, int socket)
      : name_{std::move(name)}, socket_{socket} {}

  int getSocket() const { return socket_; }

  std::string &getName() { return name_; }

private:
  std::string name_;
  int socket_;
};

int main() {
  constexpr int PORT = 8080;
  constexpr int MAX_CONNECTIONS = 10;

  int server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_fd == -1) {
    std::println(stderr, "Failed to create socket descriptor. Error: {}",
                 strerror(errno));
    return 1;
  }

  int opt = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
    std::println(stderr, "Failed to set SO_REUSEADDR option. Error: {}",
                 strerror(errno));
    return 1;
  }
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
    std::println(stderr, "Failed to set SO_REUSEPORT option. Error: {}",
                 strerror(errno));
    return 1;
  }

  struct sockaddr_in address {};
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(PORT);

  if (bind(server_fd, reinterpret_cast<struct sockaddr *>(&address),
           sizeof(address)) < 0) {
    std::println(stderr, "Failed to bind socket! Error: {}", strerror(errno));
    return 1;
  }
  std::println("Listening with protocol TCP on port: {}", PORT);

  if (listen(server_fd, MAX_CONNECTIONS) < 0) {
    std::println(stderr, "Failed to listen on socket! Error: {}",
                 strerror(errno));
    return 1;
  }

  std::vector<ClientSocket> clients;
  Message msg;
  while (true) {
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    int max_fd = server_fd;

    for (auto const &client : clients) {
      auto client_socket = client.getSocket();
      FD_SET(client_socket, &read_fds);
      max_fd = std::max(max_fd, client_socket);
    }

    int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);
    if (activity < 0) {
      std::println(stderr, "Select error");
      continue;
    }

    // Handle new connections
    if (FD_ISSET(server_fd, &read_fds)) {
      auto new_socket = accept(server_fd, nullptr, nullptr);
      if (new_socket < 0) {
        std::println(stderr, "Accept failed");
        continue;
      }

      std::string name;
      recv(new_socket, &msg, sizeof(msg), 0);
      if (strcmp(msg.message, "LOGIN") == 0) {
        name = msg.sender;
      } else {
        name = "Anonymous";
      }
      clients.emplace_back(name, new_socket);
      msg = Message{"SERVER",
                    std::format("Welcome, {}", clients.back().getName())};
      send(new_socket, &msg, sizeof(msg), 0);
    }

    // Handle client messages
    for (auto it = clients.begin(); it != clients.end();) {
      int client_socket = it->getSocket();
      if (FD_ISSET(client_socket, &read_fds)) {
        auto valread = recv(client_socket, &msg, sizeof(msg), 0);
        if (valread == 0) {
          // Client disconnected
          std::println("Client {} disconnected", client_socket);
          close(client_socket);
          it = clients.erase(it);
        } else {
          // Broadcast message to all clients
          for (auto const &client : clients) {
            auto sock = client.getSocket();
            if (sock != client_socket) {
              send(sock, &msg, sizeof(msg), 0);
            }
          }
          ++it;
        }
      } else {
        ++it;
      }
    }
  }
  return 0;
}