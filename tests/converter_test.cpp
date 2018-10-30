#include "converter.h"
#include "gtest/gtest.h"

using namespace std;
using namespace cpp_trie::const_graph;

TEST(converter, convert_node) {
  class Data {};
  using LinkP = unique_ptr<cpp_trie::non_const_graph::Link<Data>>;
  using NodeP = unique_ptr<cpp_trie::non_const_graph::Node<Data>>;

  constexpr auto graph = "a"_s;
  variant<LinkP, NodeP, nullptr_t> ptr = cpp_trie::convert<Data>(&graph);
  cpp_trie::non_const_graph::Link<Data>(std::move(ptr), nullptr, true);
}

TEST(converter, convert_graph) {
  struct Data {
    int x;
  };

  using L = cpp_trie::non_const_graph::Link<Data>;
  using N = cpp_trie::non_const_graph::Node<Data>;
  constexpr auto graph =
      "a"_s + "b"_s + ("c"_s | "d"_s([](auto *d) { d->x += 1; })) + "e"_s;
  auto ptr = cpp_trie::convert<Data>(&graph);
  ASSERT_EQ(std::get<const N *>(ptr->get_next()->get_content())->get_str(),
            "b");
}