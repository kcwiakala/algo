#include <limits>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Graph.hpp>

namespace algo {
namespace bf {

constexpr int MAX_INT = std::numeric_limits<int>::max();

struct Graph: public GenericGraph<WeightedEdge>
{
  Graph(size_t s): GenericGraph(s) {}

  VerticeList bellmanFord(int source)
  {
    VerticeList distance(size, MAX_INT);
    distance[source] = 0;

    for(int i=0; i<size-1; ++i) {
      for(int u=0;u<size;++u) {
        // If distance from u is INF there is no point checking its adjacency yet
        if(distance[u] == MAX_INT) {
          continue;
        }
        // Over each edge of the graph
        for(const auto& edge: adjacency[u]) {
          int v = edge.to;
          int aux = distance[u] + edge.weight;
          if(aux < distance[v]) {
            distance[v] = aux;
          }
        }
      }
    }

    // Detect negative cycles
    for(int u=0;u<size;++u) {
      if(distance[u] == MAX_INT) { 
        // u not reachable from source
        continue;
      }
      for(const auto& edge: adjacency[u]) {
        int v = edge.to;
        if(distance[v] > distance[u] + edge.weight) {
          distance[source] = MAX_INT;
          return distance;
        }
      }
    }

    return distance;
  }
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

  auto distance = g.bellmanFord(0);
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

  auto distance = g.bellmanFord(0);
  ASSERT_EQ(distance.size(), 3);
  EXPECT_EQ(distance[0], MAX_INT);  
}

} // namespace bf
} // namespace algo