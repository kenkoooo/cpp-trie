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

template <typename Data, typename InnerData> class Wrapper : public Node<Data> {
private:
  using Setup = std::function<void(const Data *, InnerData *)>;
  using TearDown = std::function<void(Data *, const InnerData &)>;

  const Link<InnerData> *ptr;

public:
  const Setup setup;
  const TearDown tear_down;

  constexpr Wrapper(Link<InnerData> *ptr, Setup setup, TearDown tear_down)
      : ptr(ptr), setup(setup), tear_down(tear_down) {}

  virtual std::vector<Data> propagate(const Data &init_data) const
      noexcept override {
    InnerData init_inner;
    init_inner.pos = init_data.pos;
    init_inner.weight = init_data.weight;
    init_inner.input = init_data.input;
    this->setup(&init_data, &init_inner);

    std::vector<InnerData> inner_results = search_link(ptr, init_inner);
    std::vector<Data> results;
    for (const auto &inner : inner_results) {
      Data data = init_data;
      data.pos = inner.pos;
      data.weight = inner.weight;
      this->tear_down(&data, inner);
      results.push_back(data);
    }
    return results;
  }
};

} // namespace cpp_trie::non_const_graph