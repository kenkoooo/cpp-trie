#pragma once

#include "non_const_graph.h"

#include <cassert>
#include <vector>

namespace cpp_trie::search {
template <typename Data> using Node = cpp_trie::non_const_graph::Node<Data>;
template <typename Data> using Link = cpp_trie::non_const_graph::Link<Data>;

template <typename Data>
constexpr std::vector<Data> search_link(const Link<Data> *link,
                                        const Data &data);
template <typename Data>
constexpr std::vector<Data> search_node(const Node<Data> *node,
                                        const Data &data);

template <typename Data>
constexpr std::vector<Data>
search(std::variant<const Link<Data> *, const Node<Data> *, std::nullptr_t> v,
       const Data &data) {
  if (std::holds_alternative<const Link<Data> *>(v)) {
    const Link<Data> *ptr = std::get<const Link<Data> *>(v);
    assert(ptr != nullptr);
    return search_link<Data>(ptr, data);
  } else if (std::holds_alternative<const Node<Data> *>(v)) {
    const Node<Data> *ptr = std::get<const Node<Data> *>(v);
    assert(ptr != nullptr);
    return search_node<Data>(ptr, data);
  } else {
    return {data};
  }
}

template <typename Data>
constexpr std::vector<Data> search_link(const Link<Data> *link,
                                        const Data &data) {
  if (link == nullptr) {
    return {data};
  } else if (link->is_horizontal()) {
    auto tmp_results = search(link->get_content(), data);

    std::vector<Data> results;
    for (const auto &tmp : tmp_results) {
      auto t = search_link(link->get_next(), tmp);
      std::copy(t.begin(), t.end(), std::back_inserter(results));
    }
    return results;
  } else {
    std::vector<Data> results;
    auto tmp1 = search(link->get_content(), data);
    std::copy(tmp1.begin(), tmp1.end(), std::back_inserter(results));

    if (link->has_next()) {
      auto tmp2 = search_link(link->get_next(), data);
      std::copy(tmp2.begin(), tmp2.end(), std::back_inserter(results));
    }
    return results;
  }
}

template <typename Data>
constexpr std::vector<Data> search_node(const Node<Data> *node,
                                        const Data &data) {
  if (data.matched(node->get_str())) {
    auto next_pos = data.pos + node->get_str().size();
    auto next_weight = data.weight * node->get_weight()(&data);

    auto &input = data.input;
    if (input.size() > next_pos && input[next_pos] == ' ') {
      next_pos += 1;
    }

    if (next_weight > 0.0) {
      Data next_data = data;
      next_data.pos = next_pos;
      next_data.weight = next_weight;
      node->get_callback()(&next_data);
      return {next_data};
    }
  }
  return {};
}

} // namespace cpp_trie::search
