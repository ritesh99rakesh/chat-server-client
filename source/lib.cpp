#include "lib.hpp"
#include <algorithm>

Message::Message() {}

Message::Message(std::string_view _sender, std::string_view _message) {
  std::fill(sender, sender + sizeof(sender), 0);
  std::fill(message, message + sizeof(message), 0);
  std::copy(_sender.begin(), _sender.end(), sender);
  std::copy(_message.begin(), _message.end(), message);
  message_len = _message.size();
}
