#pragma once

#include <cstddef>
#include <string_view>

struct Message {
  char sender[32];
  char message[256];
  size_t message_len;
  Message();
  Message(std::string_view sender, std::string_view message);
};
