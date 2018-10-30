#pragma once

#include <string_view>

namespace cpp_trie {
class Node {
private:
  const std::string_view str;

public:
  Node(const std::string_view str);
  constexpr auto get_str() const noexcept { return this->str; }
};
} // namespace cpp_trie