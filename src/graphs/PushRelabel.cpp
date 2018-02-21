#include <queue> 
#include <list>

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
        connect(u, e.to, e.capacity);
        connect(e.to, u, 0);
      }
    } 
    
    vertices.assign(size, {0,0});
    vertices[source].height = size;
    for(auto& e: adjacency[source]) {
      const int capacity = e.capacity;
      e.flow = capacity;
      e.capacity = 0;
      auto& reverse = edge(e.to, source);
      reverse.capacity = capacity;
      vertices[e.to].excess = capacity;
      vertices[source].excess -= capacity;
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

  bool push(int i, ResidualEdge& e) 
  {
    Vertice& u = vertices[i];
    Vertice& v = vertices[e.to];
    if((e.capacity > 0) && (u.height == v.height + 1)) {
      int df = std::min(u.excess, e.capacity);
      e.flow += df;
      e.capacity -= df;
      auto& reverse = edge(e.to, i);
      reverse.flow -= df;
      reverse.capacity += df;
      u.excess -= df;
      v.excess += df;
      // std::cout << "Pushing " << df << " from " << i << " to " << e.to << std::endl;
      return true;
    } 
    return false;
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
          if(push(i, e)) {
            return true;
          }
        }
      }
    }
    return false;
  }

  bool relabel(int i) 
  {
    Vertice& u = vertices[i];
    if(u.excess > 0) {
      // std::cout << i << " has excess " << u.excess << std::endl;
      int minAdjHeight = MAX_INT;
      for(const auto& e: adjacency[i]) {
        // std::cout << "Edge to " << e.to << " has capacity " <<  e.capacity << std::endl;
        if(e.capacity > e.flow) {
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

  bool relabel()
  {
    for(int i=0; i<size; ++i) {
      if(i == source || i == sink) {
        continue;
      } else if(relabel(i)) {
        return true;
      }
    }
    return false;
  }

  void discharge(int i) 
  {
    Vertice& u = vertices[i];
    auto eit = adjacency[i].begin();
    while(u.excess > 0) {
      if(eit == adjacency[i].end()) {
        relabel(i);
        eit = adjacency[i].begin();
      } else if (!push(i, *eit)) {
        ++eit;
      }
    }
  }

  void relabelToFront() 
  {
    std::list<int> list;
    for(int i=0; i<size; ++i) {
      if(i != source && i != sink) {
        list.push_back(i);
      }
    }
    auto it = list.begin();
    while(it != list.end()) {
      int i = *it;
      Vertice& u = vertices[i];
      int oldHeight = u.height;
      discharge(i);
      if(u.height > oldHeight) {
        list.erase(it);
        list.push_front(i);
        it = list.begin();
      }
      ++it;
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

  const int source;
  const int sink;
  std::vector<Vertice> vertices;
};

struct Graph: public GenericGraph<CapacityEdge>
{
  Graph(size_t s): GenericGraph(s) {}

  int maxFlow(int source, int sink, bool toFront = true)
  {
    ResidualFlowGraph rg(adjacency, source, sink);
    if(toFront) {
      rg.relabelToFront();
    } else {
      while(rg.push() || rg.relabel());
    }
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

  EXPECT_THAT(g.maxFlow(0,5, true), testing::Eq(23));
  EXPECT_THAT(g.maxFlow(0,5, false), testing::Eq(23));
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

  EXPECT_THAT(g.maxFlow(0,4,true), testing::Eq(20));
  EXPECT_THAT(g.maxFlow(0,4,false), testing::Eq(20));
}

} // push_relabel
} // namespace algo