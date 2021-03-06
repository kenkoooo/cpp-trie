#include "converter.h"
#include "non_const_graph.h"
#include "search.h"
#include "utils.h"
#include "wrapper.h"
#include "gtest/gtest.h"

using namespace std;
using namespace cpp_trie;
using namespace cpp_trie::const_graph;

TEST(search, search_test) {
  constexpr auto graph =
      "a"_s + "b"_s + "c"_s +
      ("d"_s | "e"_s([](auto *d) { d->ok = true; }) + "f"_s) + "g"_s;

  struct Data : public utils::BlockData {
    bool ok = false;
  };

  {
    Data t;
    t.pos = 0;
    t.weight = 1.0;
    t.input = "a b c d g";
    auto ptr = convert<Data>(&graph);
    const auto p = ptr.get();
    auto result = search::search_link(ptr.get(), t);
    ASSERT_EQ(result.size(), 1);
    ASSERT_FALSE(result[0].ok);
    ASSERT_EQ(result[0].pos, 9);
  }

  {
    Data t;
    t.pos = 0;
    t.weight = 1.0;
    t.input = "a b c e f g";
    auto ptr = convert<Data>(&graph);
    const auto p = ptr.get();
    auto result = search::search_link(ptr.get(), t);
    ASSERT_EQ(result.size(), 1);
    ASSERT_TRUE(result[0].ok);
    ASSERT_EQ(result[0].pos, 11);
  }
}

TEST(search, wrapper_search) {
  struct DataA : utils::BlockData {
    bool match = false;
  };
  struct DataB : utils::BlockData {
    shared_ptr<DataA> a;
  };

  auto graph_a =
      "a"_s + "b"_s + ("c"_s | "d"_s([](auto *d) { d->match = true; }));
  auto ptr_a = convert<DataA>(&graph_a);

  auto graph_b = "e"_s +
                 ConstWrapper(ptr_a.get(),
                              [](const auto *b, auto *a) {},
                              [](auto *b, const auto &a) {
                                b->a = make_shared<DataA>(a);
                              }) +
                 "f"_s;
  auto ptr_b = convert<DataB>(&graph_b);
  {
    DataB b;
    b.input = "e a b c f";
    b.pos = 0;
    b.weight = 1.0;
    auto results = search::search_link(ptr_b.get(), b);
    ASSERT_EQ(results.size(), 1);
    ASSERT_FALSE(results[0].a->match);
  }
  {
    DataB b;
    b.input = "e a b d f";
    b.pos = 0;
    b.weight = 1.0;
    auto results = search::search_link(ptr_b.get(), b);
    ASSERT_EQ(results.size(), 1);
    ASSERT_TRUE(results[0].a->match);
  }
}
