#include <algorithm>
#include <queue>
#include <vector>

namespace algo {

struct GenericEdge
{
  int to;
};

struct WeightedEdge
{
  int to;
  int weight;
};

template<typename EDGE=GenericEdge, bool DIRECTED=true>
class GenericGraph
{
public:
  using EdgeType=EDGE;
  using VerticeList=std::vector<int>;
  using Flags=std::vector<bool>;

  GenericGraph(size_t s): size(s), adjacency(size) {}
  GenericGraph(const GenericGraph& rhs) = default;

  template<typename... Args>
  void connect(int u, int v, Args&&... args)
  {
    adjacency[u].push_back({v, std::forward<Args>(args)...});
    if(!DIRECTED) {
      adjacency[v].push_back({u, std::forward<Args>(args)...});
    }
  }

  void transpose()
  {
    if(DIRECTED) {
      std::vector<EdgeList> aux(size);
      aux.swap(adjacency);
      for(int u=0; u<size; ++u) {
        for(const auto& e: aux[u]) {
          connect(e.to,u);
        }
      }
    }
  }

  struct TraversalState
  {
    TraversalState(size_t size): 
      discovered(size, false), 
      processed(size, false), 
      parent(size, -1)
    {}

    virtual void reset()
    {
      discovered.assign(discovered.size(), false);
      processed.assign(processed.size(), false);
      parent.assign(parent.size(), -1);
    }

    Flags discovered;
    Flags processed;
    VerticeList parent;

    virtual void processEarly(int u) {}
    virtual void processLate(int u) {}
    virtual void processEdge(int v, const EdgeType& e) {}
    virtual bool validEdge(const EdgeType& e) { return true; }
  };

  void dfsImpl(int u, TraversalState& s)
  {
    s.discovered[u] = true;
    s.processEarly(u);

    for(const EdgeType& e: adjacency[u]) {
      const int v = e.to;
      if(!s.discovered[v] && s.validEdge(e)) {
        s.parent[v] = u;
        s.processEdge(u,e);
        dfsImpl(v, s);
      } else if (!s.processed[v] || DIRECTED) {
        s.processEdge(u,e);
      }
    }
    s.processLate(u);
    s.processed[u] = true;
  }

  void bfsImpl(int start, TraversalState& s)
  {
    std::queue<int> queue;
    queue.push(start);

    s.discovered[start] = true;

    while(!queue.empty()) {
      const int u = queue.front();
      queue.pop();
      s.processEarly(u);
      s.processed[u] = true;
      for(const EdgeType& e: adjacency[u]) {
        const int v = e.to;
        if(!s.processed[v] || DIRECTED) {
          s.processEdge(u,e);
        }
        if(!s.discovered[v] && s.validEdge(e)) {
          queue.push(v);
          s.discovered[v] = true;
          s.parent[v] = u;
        }
      }
      s.processLate(u);
    }
  }

  VerticeList buildPath(int u, int v, const VerticeList& parent) const
  {
    VerticeList result;
    result.push_back(v);

    int x = v;
    while(x != u) {
      if(parent[x] < 0) {
        return VerticeList();
      }
      x = parent[x];
      result.push_back(x);
    } 
    std::reverse(result.begin(), result.end());
    return result;
  }

protected:
  using EdgeList=std::vector<EdgeType>;

  const size_t size;
  std::vector<EdgeList> adjacency;
};

} // namespace algo