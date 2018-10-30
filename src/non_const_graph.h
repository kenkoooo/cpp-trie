#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <string_view>
#include <variant>

namespace cpp_trie::non_const_graph {

template <typename Data> class Node {
private:
  using Callback = std::function<void(Data *)>;
  using Weight = std::function<double(const Data *)>;

  const std::string_view str;
  const Callback callback;
  const Weight weight;

public:
  constexpr Node(const std::string_view str,
                 const Callback callback,
                 const Weight weight)
      : str(str), callback(callback), weight(weight) {}

  constexpr auto get_str() const noexcept { return this->str; }
  constexpr auto get_callback() const noexcept { return this->callback; }
  constexpr auto get_weight() const noexcept { return this->weight; }
};

template <typename Data> class Link {
private:
  using LinkP = std::unique_ptr<Link<Data>>;
  using NodeP = std::unique_ptr<Node<Data>>;
  using VariantP = std::variant<LinkP, NodeP, std::nullptr_t>;

  VariantP content;
  LinkP next;
  bool _is_horizontal;

public:
  constexpr Link(VariantP content, LinkP next, bool _is_horizontal)
      : content(std::move(content)),
        next(std::move(next)),
        _is_horizontal(_is_horizontal) {}

  constexpr bool has_next() const noexcept { return this->next != nullptr; }

  constexpr std::variant<const Link<Data> *, const Node<Data> *, std::nullptr_t>
  get_content() const noexcept {
    if (std::holds_alternative<LinkP>(this->content)) {
      return std::get<LinkP>(this->content).get();
    } else if (std::holds_alternative<NodeP>(this->content)) {
      return std::get<NodeP>(this->content).get();
    } else {
      return nullptr;
    }
  }
  constexpr bool is_horizontal() const noexcept { return this->_is_horizontal; }
  constexpr const Link<Data> *get_next() const noexcept {
    return this->next.get();
  }
};
} // namespace cpp_trie::non_const_graph