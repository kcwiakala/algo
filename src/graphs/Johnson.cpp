#include <list>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Graph.hpp>
#include <Matrix.hpp>

namespace algo {
namespace johnson {

constexpr int MAX_INT = std::numeric_limits<int>::max();

struct Graph: public GenericGraph<WeightedEdge>
{
  Graph(size_t s): GenericGraph(s) {}

  VerticeList bellmanFord(int s, const Graph& g) 
  {
    VerticeList distance(g.size, MAX_INT);
    distance[s] = 0;
    for(int i=0; i<g.size-1; ++i) {
      for(int u=0; u<g.size; ++u) {
        if(distance[u] == MAX_INT) {
          continue;
        }
        for(const auto& e: g.adjacency[u]) {
          int v = e.to;
          distance[v] = std::min(distance[v], distance[u] + e.weight);
        }
      }
    }
    // skip cycle check
    return distance;
  }

  auto dijkstra(int s, const Graph& g)
  {
    Flags onHeap(g.size, false);
    Flags visited(g.size, false);
    VerticeList distance(g.size, MAX_INT);
    VerticeList parent(g.size, -1);
    distance[s] = 0;

    auto comp = [&distance](int v1, int v2) {
      return distance[v1] > distance[v2];
    };

    std::vector<int> heap;
    heap.push_back(s);
    onHeap[s] = true;

    while(!heap.empty()) {
      int u = heap.front();
      std::pop_heap(heap.begin(), heap.end());
      heap.pop_back();
      onHeap[u] = false;
      visited[u] = true;

      bool rebuildHeap = false;
      for(const auto& e: g.adjacency[u]) {
        const int v = e.to;
        if(!visited[v]) {
          int aux = distance[u] + e.weight;
          if(aux < distance[v]) {
            parent[v] = u;
            distance[v] = aux;
            if(onHeap[v]) {
              rebuildHeap = true;
            } else {
              heap.push_back(v);
              std::push_heap(heap.begin(), heap.end());
              onHeap[v] = true;
            }
          }
        }
      }
      if(rebuildHeap) {
        std::make_heap(heap.begin(), heap.end());
      }
    }
    return std::make_pair(distance, parent);
  }

  auto johnson() 
  {
    Graph g(size+1);
    std::copy(adjacency.begin(), adjacency.end(), g.adjacency.begin());
    int s = size;
    for(int i=0; i<size; ++i) {
      g.connect(s, i, 0);
    }
    
    VerticeList d = bellmanFord(s, g);

    // re-weight
    for(int u=0; u<size; ++u) {
      for(auto& e: g.adjacency[u]) {
        const int v = e.to;
        e.weight = e.weight + d[u] - d[v];
      }
    }

    Matrix<int> D(size, size, MAX_INT);
    Matrix<int> P(size, size, -1);

    for(int u=0; u<size; ++u) {
      auto dp = dijkstra(u, g);
      for(int v=0; v<size; ++v) {
        D(u,v) = dp.first[v] + d[v] - d[u];
        P(u,v) = dp.second[v];
      }
    }

    return std::make_pair(D,P);
  }
};

auto path(int i, int j, const Matrix<int>& parent) 
{
  std::list<int> result;
  result.push_front(j);
  int p = j;
  while(p != i) {
    p = parent(i,p);
    result.push_front(p);
  }

  return result;
}

TEST(FloydWarshall, test1) 
{
  Graph g(5);
  g.connect(0,1,3);
  g.connect(0,2,8);
  g.connect(0,4,-4);
  g.connect(1,3,1);
  g.connect(1,4,7);
  g.connect(2,1,4);
  g.connect(3,0,2);
  g.connect(3,2,-5);
  g.connect(4,3,6);

  auto result = g.johnson();
  const auto& dist = result.first;
  const auto& parent = result.second;

  for(int i=0; i<dist.width(); ++i) {
    EXPECT_EQ(dist(i,i), 0);
  }
  for(int i=0; i<parent.width(); ++i) {
    EXPECT_EQ(parent(i,i), -1);
  }

  // Path 4-1: 4 -> 3 -> 2 -> 1 
  EXPECT_EQ(dist(4,1), 5);
  auto p = path(4,1, parent);
  EXPECT_THAT(p.size(), testing::Eq(4));
  EXPECT_THAT(p, testing::ElementsAre(4,3,2,1));
}

} // namespace johnson
} // namespace algo