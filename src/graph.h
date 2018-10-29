#pragma once

#include <string_view>

namespace cpp_trie {
class Node {
private:
  const std::string_view str;

public:
  Node(const std::string_view str);
};
} // namespace cpp_trie