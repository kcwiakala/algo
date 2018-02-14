#include "GraphsCommon.hpp"

namespace algo {
namespace topo {

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

  const int size;
  std::vector<List> adjacency;
};

TEST(TolopologicalSort, test1) 
{
  Graph g(5);
  g.connect(0,2);
  g.connect(0,3);
  g.connect(2,1);
  g.connect(3,2);
  g.connect(3,4);
  g.connect(4,1);
  List sorted = g.topoSort();
  ASSERT_THAT(sorted.size(), testing::Eq(5));
  ASSERT_THAT(sorted, testing::ElementsAre(0,3,4,2,1));
}

} // namespace topo
} // namespace algo