#pragma once

#include "const_graph.h"
#include "non_const_graph.h"

namespace cpp_trie {
template <typename Data>
constexpr std::nullptr_t convert(const const_graph::Terminator *node) noexcept {
  return nullptr;
}

template <typename Data, typename C, typename W>
constexpr std::unique_ptr<non_const_graph::Node<Data>>
convert(const const_graph::Node<C, W> *node) noexcept {
  auto callback = node->get_callback();
  auto weight = node->get_weight();
  auto str = node->get_str();
  return std::make_unique<non_const_graph::Node<Data>>(str, callback, weight);
}

template <typename Data, typename Content, bool Horizontal, typename Next>
constexpr std::unique_ptr<non_const_graph::Link<Data>> convert(
    const const_graph::Link<Content, Horizontal, Next> *const_link) noexcept {
  std::variant<std::unique_ptr<non_const_graph::Link<Data>>,
               std::unique_ptr<non_const_graph::Node<Data>>,
               std::nullptr_t>
      content = convert<Data>(const_link->get_content());
  std::unique_ptr<non_const_graph::Link<Data>> next =
      convert<Data>(const_link->get_next());
  return std::make_unique<non_const_graph::Link<Data>>(
      std::move(content), std::move(next), Horizontal);
}

} // namespace cpp_trie