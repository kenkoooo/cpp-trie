#pragma once

#include <string_view>

namespace cpp_trie::utils {
struct BlockData {
  std::string_view input;
  std::size_t pos;
  double weight;

  constexpr bool matched(const std::string_view &s) const noexcept {
    return this->input.substr(this->pos, s.size()) == s;
  }
};
} // namespace cpp_trie::utils