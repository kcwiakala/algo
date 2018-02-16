#include <queue> 

#include "GraphsCommon.hpp"

namespace algo {
namespace edmonds {

struct Edge
{
  Edge(int t, int c): to(t), capacity(c) {}

  int to;
  int capacity;

  int flow = 0;
  int residual = 0;
};

using EdgeList = std::vector<Edge>;

struct ResidualFlowGraph
{
  ResidualFlowGraph(const std::vector<EdgeList>& adj): size(adj.size()), adjacency(adj)
  {
    for(int u=0; u<size; ++u) {
      for(Edge& e: adjacency[u]) {
        e.residual = e.capacity;
        adjacency[e.to].push_back({u, 0});
      }
    }
  }

  List bfs(int source) 
  {
    List parents(size, -1);
    Flags discovered(size, false);
    std::queue<int> queue;

    queue.push(source);
    while(!queue.empty()) {
      int u = queue.front();
      queue.pop();
      discovered[u] = true;
      for(const Edge& e: adjacency[u]) {
        int v = e.to;
        if(!discovered[v] && (e.residual > 0)) {
          parents[v] = u;
          discovered[v] = true;
          queue.push(v);
        }
      }
    }
    return parents;
  }

  List path(int u, int v, const List& parents)
  {
    List result;
    result.push_back(v);

    int x = v;
    while(x != u) {
      if(parents[x] < 0) {
        return List();
      }
      x = parents[x];
      result.push_back(x);
    } 
    std::reverse(result.begin(), result.end());
    return result;
  }

  int volume(const List& p)
  {
    int result = std::numeric_limits<int>::max();
    for(int i=0; i<p.size()-1;++i) {
      int u = p[i];
      int v = p[i+1];
      for(const Edge& e: adjacency[u]) {
        if(e.to == v) {
          result = std::min(result, e.residual);
          break;
        }
      }
    }
    return result;
  }

  void augment(const List& p, int vol)
  {
    for(int i=0; i<p.size()-1;++i) {
      int u = p[i];
      int v = p[i+1];
      for(Edge& e: adjacency[u]) {
        if(e.to == v) {
          e.flow += vol;
          e.residual -= vol;
          break;
        }
      }
      for(Edge& e: adjacency[v]) {
        if(e.to == u) {
          e.residual += vol;
          break;
        }
      }
    }
  }

  int flow(int source) 
  {
    int f = 0;
    for(const Edge& e: adjacency[source]) {
      f += e.flow;
    }
    return f;
  }

  const size_t size;
  std::vector<EdgeList> adjacency;
};

struct Graph
{
  Graph(size_t s): size(s), adjacency(size) {}

  void connect(int u, int v, int c)
  {
    adjacency[u].emplace_back(v,c);
  }

  int maxFlow(int source, int sink)
  {
    ResidualFlowGraph rg(adjacency);

    List parents = rg.bfs(source);
    List augmentingPath = rg.path(source, sink, parents);

    while(!augmentingPath.empty()) {
      int augmentingVolume = rg.volume(augmentingPath);
      rg.augment(augmentingPath, augmentingVolume);
      parents = rg.bfs(source);
      augmentingPath = rg.path(source, sink, parents);
    }

    return rg.flow(source);
  }

  const size_t size;
  std::vector<EdgeList> adjacency;
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