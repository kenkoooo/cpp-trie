#pragma once

#include "const_graph.h"
#include "non_const_graph.h"
#include "search.h"

namespace cpp_trie {

template <typename Data, typename InnerData>
class Wrapper : public non_const_graph::Node<Data> {
private:
  using Setup = std::function<void(const Data *, InnerData *)>;
  using TearDown = std::function<void(Data *, const InnerData &)>;
  using L = non_const_graph::Link<InnerData>;

  const L *ptr;

public:
  const Setup setup;
  const TearDown tear_down;

  constexpr Wrapper(const L *ptr, const Setup setup, const TearDown tear_down)
      : ptr(ptr), setup(setup), tear_down(tear_down) {}

  virtual std::vector<Data> propagate(const Data &init_data) const
      noexcept override {
    InnerData init_inner;
    init_inner.pos = init_data.pos;
    init_inner.weight = init_data.weight;
    init_inner.input = init_data.input;
    this->setup(&init_data, &init_inner);

    std::vector<InnerData> inner_results = search::search_link(ptr, init_inner);
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

template <typename InnerData, typename Setup, typename TearDown>
class ConstWrapper {
private:
  using L = non_const_graph::Link<InnerData>;

public:
  const L *ptr;
  const Setup setup;
  const TearDown tear_down;

  constexpr ConstWrapper(const L *ptr,
                         const Setup setup,
                         const TearDown tear_down)
      : ptr(ptr), setup(setup), tear_down(tear_down) {}
};

template <typename Data, typename InnerData, typename S, typename T>
constexpr std::unique_ptr<non_const_graph::Node<Data>>
convert(const ConstWrapper<InnerData, S, T> *wrapper) noexcept {
  const non_const_graph::Link<InnerData> *ptr = wrapper->ptr;
  auto setup = wrapper->setup;
  auto tear_down = wrapper->tear_down;
  return std::make_unique<Wrapper<Data, InnerData>>(ptr, setup, tear_down);
}

} // namespace cpp_trie