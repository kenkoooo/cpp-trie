#include "converter.h"
#include "non_const_graph.h"
#include "search.h"
#include "utils.h"
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
  }
}
