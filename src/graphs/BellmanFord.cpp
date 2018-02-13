#include "GraphsCommon.hpp"

namespace algo {

struct Edge
{
  Edge(int t, int w): target(t), weight(w) {}

  int target;
  int weight;
};

using EdgeList=std::vector<Edge>;

struct Graph
{
  Graph(int size): N(size), adj(size) {}

  void connect(int u, int v, int w)
  {
    adj[u].emplace_back(v,w);
  }

  List bellmanFord(int source)
  {
    List distance(N, MAX_INT);  // Algorithm result, distances from source to vertices
    distance[source] = 0;

    // Loop N-1 times
    for(int i=0; i<N-1; ++i) {
      for(int u=0;u<N;++u) {
        // If distance from u is INF there is no point checking its adjacency yet
        if(distance[u] == MAX_INT) {
          continue;
        }
        // Over each edge of the graph
        for(const auto& edge: adj[u]) {
          int v = edge.target;
          int aux = distance[u] + edge.weight;
          if(aux < distance[v]) {
            distance[v] = aux;
          }
        }
      }
    }

    // Detect negative cycles
    for(int u=0;u<N;++u) {
      if(distance[u] == MAX_INT) { 
        // u not reachable from source
        continue;
      }
      for(const auto& edge: adj[u]) {
        int v = edge.target;
        if(distance[v] > distance[u] + edge.weight) {
          distance[source] = MAX_INT;
          return distance;
        }
      }
    }
    

    return distance;
  }

  const int N;
  std::vector<EdgeList> adj;
};

TEST(BellmanFord, test1) 
{
  Graph g(6);
  g.connect(0,1,3);
  g.connect(0,5,7);
  g.connect(1,2,4);
  g.connect(1,4,2);
  g.connect(3,4,6);
  g.connect(5,2,3);
  g.connect(5,4,1);

  List distance = g.bellmanFord(0);
  ASSERT_EQ(distance.size(), 6);
  EXPECT_EQ(distance[0], 0);
  EXPECT_EQ(distance[1], 3);
  EXPECT_EQ(distance[2], 7);
  EXPECT_EQ(distance[3], MAX_INT);
  EXPECT_EQ(distance[4], 5);
  EXPECT_EQ(distance[5], 7);
}

TEST(bellmanFord, negative_cycle)
{
  Graph g(3);
  g.connect(0,1,-1);
  g.connect(1,2,-3);
  g.connect(2,0,2);

  List distance = g.bellmanFord(0);
  ASSERT_EQ(distance.size(), 3);
  EXPECT_EQ(distance[0], MAX_INT);  
}

} // namespace algo