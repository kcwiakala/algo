#include "GraphsCommon.hpp"

#include <Graph.hpp>

namespace algo {
namespace scc {

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

  auto scc()
  {
    Graph gt(*this);
    gt.transpose();

    struct State: public TraversalState
    {
      State(size_t s): TraversalState(s) {}
      void processLate(int u) override
      {
        components.back().push_back(u);
      }
      std::vector<VerticeList> components;
    };

    State state(size);
    for(int s: topologicalSort()) {
      if(!state.processed[s]) {
        state.components.emplace_back();
        gt.dfsImpl(s, state);
      }
    }
    return state.components;
  }
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