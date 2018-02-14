#include <queue>

#include "GraphsCommon.hpp"

namespace algo {
namespace dijkstra {
  
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

  List dijkstra(int source)
  {
    Flags known(N, false);      // Vertices already taken by greedy approach
    Flags onHeap(N, false);     // Helper flag indicating is vertice is already on heap
    List distance(N, MAX_INT);  // Algorithm result, distances from source to vertices

    distance[source] = 0; // Distance from source to source

    // Comparator to make min_heap
    auto comp = [&distance](int v1, int v2) {
      return distance[v1] > distance[v2];
    };

    // Push start vertice on the heap
    std::vector<int> heap; 
    heap.push_back(source);
    onHeap[source] = true;

    while(!heap.empty()) {
      // Pop item from heap
      int u = heap.front();
      std::pop_heap(heap.begin(), heap.end(), comp);
      heap.pop_back();
      onHeap[u] = false;

      // Mark vertice as visited
      known[u] = true;

      // Helper indicator to know if we need to rebuild the heap
      bool rebuildHeap = false;

      // Loop over adjacency list
      for(const auto& edge: adj[u]) {
        int v = edge.target;
        // If the vertice was not yet visited
        if(!known[v]) {
          // New potential distance to v
          int aux = distance[u] + edge.weight;
          if(aux < distance[v]) { 
            // Shorter path found
            distance[v] = aux;

            if(onHeap[v]) { 
              // Distance updated need to rebuild heap
              rebuildHeap = true;
            } else {
              // Push new vertice on heap
              heap.push_back(v);
              std::push_heap(heap.begin(), heap.end(), comp);
              onHeap[v] = true;
            }
          }
        }
      }
      if(rebuildHeap) {
        std::make_heap(heap.begin(), heap.end(), comp);
      }
    }


    return distance;
  }

  const int N;
  std::vector<EdgeList> adj;
};

TEST(Dijkstra, test1) 
{
  Graph g(6);
  g.connect(0,1,3);
  g.connect(0,5,7);
  g.connect(1,2,4);
  g.connect(1,4,2);
  g.connect(3,4,6);
  g.connect(5,2,3);
  g.connect(5,4,1);

  List distance = g.dijkstra(0);
  ASSERT_EQ(distance.size(), 6);
  EXPECT_EQ(distance[0], 0);
  EXPECT_EQ(distance[1], 3);
  EXPECT_EQ(distance[2], 7);
  EXPECT_EQ(distance[3], MAX_INT);
  EXPECT_EQ(distance[4], 5);
  EXPECT_EQ(distance[5], 7);
}

} // namespace dijkstra
} // namespace algo