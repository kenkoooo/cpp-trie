#pragma once

#include <cassert>
#include <functional>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

namespace cpp_trie::non_const_graph {

template <typename Data> class Node {
public:
  virtual std::vector<Data> propagate(const Data &data) const noexcept = 0;
  virtual ~Node() {}
};

template <typename Data> class SingleStringNode : public Node<Data> {
private:
  using Callback = std::function<void(Data *)>;
  using Weight = std::function<double(const Data *)>;

  const std::string_view str;

protected:
  const Callback callback;
  const Weight weight;

public:
  constexpr SingleStringNode(const std::string_view str,
                             const Callback callback,
                             const Weight weight)
      : Node<Data>(), str(str), callback(callback), weight(weight) {}

  virtual std::vector<Data> propagate(const Data &data) const
      noexcept override {
    if (data.given_string_is_matched(this->str)) {
      auto next_pos = data.pos + this->str.size();
      auto next_weight = data.weight * this->weight(&data);

      auto &input = data.input;
      if (input.size() > next_pos && input[next_pos] == ' ') {
        next_pos += 1;
      }

      if (next_weight > 0.0) {
        Data next_data = data;
        next_data.pos = next_pos;
        next_data.weight = next_weight;
        this->callback(&next_data);
        return {next_data};
      }
    }
    return {};
  }
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