#include <queue> 

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Graph.hpp>

namespace algo {
namespace push_relabel {

constexpr int MAX_INT = std::numeric_limits<int>::max();

struct ResidualEdge
{
  int to;
  int capacity;
  bool original;
  int flow = 0;
};

struct CapacityEdge
{
  int to;
  int capacity;
};

struct Vertice
{
  int height;
  int excess;
};

struct ResidualFlowGraph: public GenericGraph<ResidualEdge>
{
  template<typename EL>
  ResidualFlowGraph(const EL& adj, int s, int t): 
    GenericGraph(adj.size()), source(s), sink(t)
  {
    for(int u=0; u<size; ++u) {
      for(const auto& e: adj[u]) {
        connect(u, e.to, e.capacity, true);
        connect(e.to, u, 0, false);
      }
    }
    vertices.assign(size, {0,0});
    vertices[source].height = size;
    for(auto& e: adjacency[source]) {
      e.flow = e.capacity;
      vertices[e.to].excess = e.capacity;
      vertices[source].excess -= e.capacity;
      auto& er = edge(e.to, source);
      er.capacity = e.capacity;
      e.capacity = 0;
    }
  }

  ResidualEdge& edge(int u, int v) 
  {
    for(ResidualEdge& e: adjacency[u]) {
      if(e.to == v) {
        return e;
      }
    }
    throw std::runtime_error("Edge not found");
  }

  bool push()
  {
    for(int i=0; i<size; ++i) {
      if(i == source || i == sink) {
        continue;
      }
      Vertice& u = vertices[i];
      if(u.excess > 0) {
        for(auto& e: adjacency[i]) {
          Vertice& v = vertices[e.to];
          if((e.capacity > 0) && (u.height == v.height + 1)) {
            int df = std::min(u.excess, e.capacity);
            e.flow += df;
            e.capacity -= df;
            auto& eo = edge(e.to, i);
            eo.flow -= df;
            eo.capacity += df;
            u.excess -= df;
            v.excess += df;
            // std::cout << "Pushing " << df << " from " << i << " to " << e.to << std::endl;
            return true;
          }
        }
      }
    }
    return false;
  }

  bool relabel()
  {
    for(int i=0; i<size; ++i) {
      if(i == source || i == sink) {
        continue;
      }
      Vertice& u = vertices[i];
      if(u.excess > 0) {
        // std::cout << i << " has excess " << u.excess << std::endl;
        int minAdjHeight = MAX_INT;
        for(const auto& e: adjacency[i]) {
          // std::cout << "Edge to " << e.to << " has capacity " <<  e.capacity << std::endl;
          if(e.capacity > 0) {
            minAdjHeight = std::min(minAdjHeight, vertices[e.to].height);
          }
        }
        if(minAdjHeight < MAX_INT && minAdjHeight >= u.height) {
          u.height = minAdjHeight + 1;
          // std::cout << "Relabeling " << i << " to " << u.height << std::endl;
          return true;
        }
      }
    }
    return false;
  }

  int flow(int source) const
  {
    int f = 0;
    for(const ResidualEdge& e: adjacency[source]) {
      f += e.flow;
    }
    return f;
  }

  const int source;
  const int sink;
  std::vector<Vertice> vertices;
};

struct Graph: public GenericGraph<CapacityEdge>
{
  Graph(size_t s): GenericGraph(s) {}

  int maxFlow(int source, int sink)
  {
    ResidualFlowGraph rg(adjacency, source, sink);
    while(rg.push() || rg.relabel());
    return rg.flow(source);
  }
};

TEST(PushRelabel, test1)
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

TEST(PushRelabel, test2)
{
  Graph g(5);
  g.connect(0,1,12);
  g.connect(0,2,14);
  g.connect(1,2,5);
  g.connect(1,4,16);
  g.connect(2,3,8);
  g.connect(3,1,7);
  g.connect(3,4,10);

  EXPECT_THAT(g.maxFlow(0,4), testing::Eq(20));
}

} // push_relabel
} // namespace algo