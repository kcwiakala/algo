#include <queue> 

#include "GraphsCommon.hpp"
#include <Graph.hpp>

namespace algo {
namespace edmonds {

struct ResidualEdge
{
  ResidualEdge(int t, int r): to(t), residual(r) {}

  int to;
  int residual;
  int flow = 0;
};

struct CapacityEdge
{
  int to;
  int capacity;
};

struct ResidualFlowGraph: public GenericGraph<ResidualEdge>
{
  template<typename EL>
  ResidualFlowGraph(const EL& adj): GenericGraph(adj.size())
  {
    for(int u=0; u<size; ++u) {
      for(const auto& e: adj[u]) {
        connect(u, e.to, e.capacity);
        connect(e.to, u, 0);
      }
    }    
  }

  int volume(const VerticeList& path) const
  {
    int result = std::numeric_limits<int>::max();
    for(int i=0; i<path.size()-1;++i) {
      int u = path[i];
      int v = path[i+1];
      for(const ResidualEdge& e: adjacency[u]) {
        if(e.to == v) {
          result = std::min(result, e.residual);
          break;
        }
      }
    }
    return result;
  }

  void augment(const VerticeList& path, int vol)
  {
    for(int i=0; i<path.size()-1;++i) {
      int u = path[i];
      int v = path[i+1];
      for(ResidualEdge& e: adjacency[u]) {
        if(e.to == v) {
          e.flow += vol;
          e.residual -= vol;
          break;
        }
      }
      for(ResidualEdge& e: adjacency[v]) {
        if(e.to == u) {
          e.residual += vol;
          break;
        }
      }
    }
  }

  int flow(int source) const
  {
    int f = 0;
    for(const ResidualEdge& e: adjacency[source]) {
      f += e.flow;
    }
    return f;
  }
};

struct Graph: public GenericGraph<CapacityEdge>
{
  Graph(size_t s): GenericGraph(s) {}

  int maxFlow(int source, int sink)
  {
    ResidualFlowGraph rg(adjacency);

    struct State: public ResidualFlowGraph::TraversalState
    {
      State(size_t s): ResidualFlowGraph::TraversalState(s) {}

      bool validEdge(const ResidualEdge& e)
      {
        return e.residual > 0;
      }
    };

    State state(size);
    rg.bfsImpl(source, state);
    VerticeList augmentingPath = rg.buildPath(source, sink, state.parent);

    while(!augmentingPath.empty()) {
      int augmentingVolume = rg.volume(augmentingPath);
      rg.augment(augmentingPath, augmentingVolume);
      state.reset();
      rg.bfsImpl(source, state);
      augmentingPath = rg.buildPath(source, sink, state.parent);
    }

    return rg.flow(source);
  }
};

TEST(EdmondsKarp, test1)
{
  Graph g(6);
  g.connect(0,1,16);
  g.connect(0,2,13);
  g.connect(1,3,12);
  g.connect(2,1,4);
  g.connect(2,4,14);
  g.connect(3,2,9);
  g.connect(3,5,20);
  g.connect(4,3,7);
  g.connect(4,5,4);

  EXPECT_THAT(g.maxFlow(0,5), testing::Eq(23));
}

} // edmonds
} // namespace algo