#include "GraphsCommon.hpp"

namespace algo {
namespace scc {

struct Graph 
{
  Graph(int s): size(s), adjacency(size) {}

  void connect(int u, int v) 
  {
    adjacency[u].push_back(v);
  }

  void dfs(int s, Flags& discovered, const VerticeHandler& processLate)
  {
    discovered[s] = true;
    for(int v: adjacency[s]) {
      if(!discovered[v]) {
        dfs(v, discovered, processLate);
      } 
    }
    processLate(s);
  }

  List topoSort() 
  {
    Flags discovered(size, false);
    List sorted;
    sorted.reserve(size);

    auto processLate = [&sorted](int u) {
      sorted.push_back(u);
    };

    for(int s=0; s<size; ++s) {
      if(!discovered[s]) {
        dfs(s, discovered, processLate);
      }
    }
    std::reverse(sorted.begin(), sorted.end());

    return sorted;
  }

  auto scc()
  {
    List sorted = topoSort();

    // Transpose graph
    Graph gt(size);
    for(int u=0; u<size; ++u) {
      for(int v: adjacency[u]) {
        gt.connect(v,u);
      }
    }

    std::vector<List> components;
    auto processLate = [&components](int u) {
      components.back().push_back(u);
    };

    Flags discovered(size, false);
    for(int s: sorted) {
      if(!discovered[s]) {
        components.emplace_back();
        gt.dfs(s, discovered, processLate);
      }
    }

    return components;
  }

  const int size;
  std::vector<List> adjacency;
};

TEST(StronglyConnectedComponents, test1) 
{
  Graph g(8);
  g.connect(0,1);
  g.connect(1,2);
  g.connect(1,4);
  g.connect(1,5);
  g.connect(2,3);
  g.connect(2,6);
  g.connect(3,2);
  g.connect(3,7);
  g.connect(4,0);
  g.connect(4,5);
  g.connect(5,6);
  g.connect(6,5);
  g.connect(6,7);
  g.connect(7,7);

  auto components = g.scc();

  ASSERT_THAT(components.size(), testing::Eq(4));
  EXPECT_THAT(components[0], testing::UnorderedElementsAre(0,1,4));
  EXPECT_THAT(components[1], testing::UnorderedElementsAre(2,3));
  EXPECT_THAT(components[2], testing::UnorderedElementsAre(5,6));
  EXPECT_THAT(components[3], testing::UnorderedElementsAre(7));
}

} // namespace scc
} // namespace algo