#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Graph.hpp>

namespace algo {
namespace bipartite {

struct ResidualEdge
{
  ResidualEdge(int t, int r): to(t), residual(r) {}

  int to;
  int residual;
  int flow = 0;
};

struct ResidualFlowGraph: public GenericGraph<ResidualEdge>
{
  ResidualFlowGraph(size_t s): GenericGraph(s) {}

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
          e.flow -= vol;
          break;
        }
      }
    }
  }

  int flow(int u, int v) const
  {
    for(const ResidualEdge& e: adjacency[u]) {
      if(e.to == v) {
        return e.flow;
      }
    }
    return -1;
  }
};

struct BipartiteGraph: public GenericGraph<>
{
  BipartiteGraph(size_t s): GenericGraph(2*s) {}

  using Match=std::pair<int, int>;
  using Matching=std::vector<Match>;

  void connect(int l, int r)
  {
    const int v = size/2 + r;
    adjacency[l].push_back({v});
  }

  Matching maximumMatching()
  {
    ResidualFlowGraph rg(size+2);
    int source = size;
    int sink = size+1;
    for(int u=0; u<size/2; ++u) {
      for(const auto& e: adjacency[u]) {
        rg.connect(u, e.to, 1);
        rg.connect(e.to, u, 0);
      }
      // connect source to left side
      rg.connect(source, u, 1);
    }
    for(int u=size/2; u<size; ++u) {
      // connect right side to sink
      rg.connect(u, sink, 1);
    }

    struct State: public ResidualFlowGraph::TraversalState
    {
      State(size_t s): ResidualFlowGraph::TraversalState(s) {}

      bool validEdge(const ResidualEdge& e)
      {
        return e.residual > 0;
      }
    };

    State state(size+2);
    rg.bfsImpl(source, state);
    VerticeList augmentingPath = rg.buildPath(source, sink, state.parent);

    while(!augmentingPath.empty()) {
      int augmentingVolume = rg.volume(augmentingPath);
      rg.augment(augmentingPath, augmentingVolume);
      state.reset();
      rg.bfsImpl(source, state);
      augmentingPath = rg.buildPath(source, sink, state.parent);
    }

    Matching result;
    for(int u=0; u<size/2; ++u) {
      for(const auto& e: adjacency[u]) {
        if(rg.flow(u, e.to) > 0) {
          const int v = e.to - (size/2);
          result.emplace_back(u, v);
          break;
        }
      }
    }
    return result;
  }
};

using Match=BipartiteGraph::Match;

TEST(BipartiteMatching, test1)
{
  BipartiteGraph g(6);
  g.connect(0,0);
  g.connect(0,2);
  g.connect(1,1);
  g.connect(1,3);
  g.connect(2,0);
  g.connect(2,3);
  g.connect(3,2);
  g.connect(3,4);
  g.connect(4,4);
  g.connect(5,4);
  g.connect(5,5);

  auto match = g.maximumMatching();
  EXPECT_THAT(match, testing::ElementsAre(Match(0,0), Match(1,1), Match(2,3), Match(3,2), Match(4,4), Match(5,5)));
}

TEST(BipartiteMatching, test2) 
{
  BipartiteGraph g(3);
  g.connect(0,0);
  g.connect(0,1);
  g.connect(1,0);
  g.connect(1,2);
  g.connect(2,1);

  auto match = g.maximumMatching();
  EXPECT_THAT(match, testing::ElementsAre(Match(0,0), Match(1,2), Match(2,1)));
}

} // namespace bipartite
} // namespace algo