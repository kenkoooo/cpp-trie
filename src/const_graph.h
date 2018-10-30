#pragma once

#include <string_view>
#include <type_traits>

namespace cpp_trie::const_graph {

class Terminator {};

template <typename Callback, typename Weight> class Node {
  const std::string_view str;
  const Callback callback;
  const Weight weight;

public:
  constexpr Node(const std::string_view str, const Callback callback,
                 const Weight weight)
      : str(str), callback(callback), weight(weight) {}

  constexpr auto get_str() const noexcept { return this->str; }
  constexpr auto get_callback() const noexcept { return this->callback; }
  constexpr auto get_weight() const noexcept { return this->weight; }

  template <typename NewWeight>
  constexpr auto set_weight(const NewWeight new_weight) const noexcept {
    return Node<Callback, NewWeight>(str, callback, new_weight);
  }

  template <typename NewCallback>
  constexpr auto operator()(const NewCallback new_callback) const noexcept {
    return Node<NewCallback, Weight>(str, new_callback, weight);
  }

  template <typename NewWeight>
  constexpr auto operator[](const NewWeight new_weight) const noexcept {
    return this->set_weight(new_weight);
  }

  constexpr auto operator*(double w) const noexcept {
    auto new_weight = [w](const auto *) { return w; };
    return this->set_weight(new_weight);
  }
};

constexpr auto create_single_node(const std::string_view str) {
  constexpr auto callback = [](auto *) {};
  constexpr auto weight = [](const auto *) { return 1.0; };
  return Node(str, callback, weight);
}

constexpr auto EMPTY_NODE = create_single_node("");

constexpr auto operator"" _s(const char *str, std::size_t len) {
  return create_single_node(std::string_view(str, len));
}

template <typename T> constexpr auto opt(T t) { return t | EMPTY_NODE; }

template <typename T, typename S>
constexpr auto operator*(double w, const Node<T, S> node) {
  return node * w;
}

template <typename Content, bool Horizontal, typename Next = Terminator>
class Link {
private:
  const Content content;
  const Next next;

public:
  constexpr Link(const Content content, const Next next = {})
      : content(content), next(next) {}

  constexpr auto *get_content() const noexcept { return &this->content; }
  constexpr auto *get_next() const noexcept { return &this->next; }

  constexpr bool is_horizontal() const noexcept { return Horizontal; }
  constexpr bool is_vertical() const noexcept { return not Horizontal; }
  constexpr bool has_next() const noexcept {
    return not std::is_same_v<Next, Terminator>;
  }

  template <typename NewNext>
  constexpr auto add_next(const NewNext new_next) const noexcept {
    if constexpr (std::is_same_v<Next, Terminator>) {
      Link<NewNext, Horizontal> link(new_next);
      return Link<Content, Horizontal, decltype(link)>(content, link);
    } else {
      auto link = next.add_next(new_next);
      return Link<Content, Horizontal, decltype(link)>(content, link);
    }
  }

  template <typename NewNext>
  constexpr auto operator+(const NewNext new_next) const noexcept {
    if constexpr (Horizontal) {
      return this->add_next(new_next);
    } else {
      Link<Content, false, Next> vertical(content, next);
      Link<decltype(vertical), true> horizontal(vertical);
      return horizontal + new_next;
    }
  }

  template <typename NewNext>
  constexpr auto operator|(const NewNext new_next) const noexcept {
    if constexpr (Horizontal) {
      Link<Content, true, Next> horizontal(content, next);
      Link<decltype(horizontal), false> vertical(horizontal);
      return vertical | new_next;
    } else {
      return this->add_next(new_next);
    }
  }

  template <typename Callback>
  constexpr auto operator()(Callback callback) const noexcept {
    return Link<Content, Horizontal, Next>(content, next) +
           EMPTY_NODE(callback);
  }

  constexpr auto operator*(double weight) const noexcept {
    return Link<Content, Horizontal, Next>(content, next) + EMPTY_NODE * weight;
  }
};

template <typename T, typename U, typename V>
constexpr auto operator+(Node<T, U> node, V v) {
  return Link<decltype(node), true>(node) + v;
}

template <typename T, typename U, typename V>
constexpr auto operator|(Node<T, U> node, V v) {
  return Link<decltype(node), false>(node) | v;
}

template <typename Content, bool Horizontal, typename Next>
constexpr auto operator*(double w, const Link<Content, Horizontal, Next> link) {
  return link + EMPTY_NODE * w;
}

} // namespace cpp_trie::const_graph