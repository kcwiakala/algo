#include <Graph.hpp>

#include "GraphsCommon.hpp"

namespace algo {
namespace topo {

struct Graph: public GenericGraph<>
{
  Graph(size_t s): GenericGraph(s) {}

  VerticeList topologicalSort()
  {
    struct State: public TraversalState
    {
      State(size_t s): TraversalState(s)
      {
        sorted.reserve(s);
      }

      void processLate(int u) override
      {
        sorted.push_back(u);
      }

      VerticeList sorted;
    };

    State state(size);
    for(int u=0; u<size; ++u) {
      if(!state.processed[u]) {
        dfsImpl(u, state);
      }
    }
    std::reverse(state.sorted.begin(), state.sorted.end());
    return state.sorted;
  }
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
  List sorted = g.topologicalSort();
  ASSERT_THAT(sorted.size(), testing::Eq(5));
  ASSERT_THAT(sorted, testing::ElementsAre(0,3,4,2,1));
}

} // namespace topo
} // namespace algo