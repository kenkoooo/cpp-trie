#include "const_graph.h"
#include "gtest/gtest.h"

using namespace std;
using namespace cpp_trie::const_graph;

TEST(const_graph, construct) {
  constexpr auto graph = "a"_s + ("b"_s | "c"_s | "d"_s + "e"_s) + "f"_s;
  static_assert(graph.is_horizontal());
  static_assert(graph.get_content()->get_str() == "a");

  static_assert(graph.get_next()->is_horizontal());
  static_assert(graph.get_next()->get_content()->is_vertical());
  static_assert(graph.get_next()->get_content()->get_content()->get_str() ==
                "b");
  static_assert(
      graph.get_next()->get_content()->get_next()->get_content()->get_str() ==
      "c");
  static_assert(
      not graph.get_next()->get_content()->get_next()->get_next()->has_next());
  static_assert(graph.get_next()
                    ->get_content()
                    ->get_next()
                    ->get_next()
                    ->get_content()
                    ->is_horizontal());
  static_assert(graph.get_next()
                    ->get_content()
                    ->get_next()
                    ->get_next()
                    ->get_content()
                    ->get_content()
                    ->get_str() == "d");
  static_assert(graph.get_next()
                    ->get_content()
                    ->get_next()
                    ->get_next()
                    ->get_content()
                    ->get_next()
                    ->get_content()
                    ->get_str() == "e");
  static_assert(graph.get_next()->get_next()->get_content()->get_str() == "f");
}

TEST(const_graph, callback) {
  constexpr auto graph = "a"_s + "b"_s([](auto *t) { t->x += 1; });

  struct T {
    int x = 10;
  };
  T t;

  ASSERT_EQ(t.x, 10);
  graph.get_next()->get_content()->get_callback()(&t);
  ASSERT_EQ(t.x, 11);
}
