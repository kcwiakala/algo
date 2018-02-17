#include <functional>
#include <queue>
#include <vector>

namespace algo {

struct GenericEdge
{
  int to;
};

template<typename EDGE=GenericEdge, bool DIRECTED=true>
class GenericGraph
{
public:
  using VerticeList=std::vector<int>;
  using Flags=std::vector<bool>;

  GenericGraph(size_t s): size(s), adjacency(size) {}

  template<typename... Args>
  void connect(int u, int v, const Args&... args)
  {
    adjacency[u].push_back({v, args...});
    if(!DIRECTED) {
      adjacency[v].push_back({u, args...});
    }
  }

protected:
  struct TraversalState
  {
    TraversalState(size_t size): 
      discovered(size, false), 
      processed(size, false), 
      parent(size, -1)
    {}

    Flags discovered;
    Flags processed;
    VerticeList parent;

    virtual void processEarly(int u) {}
    virtual void processLate(int u) {}
    virtual void processEdge(int v, const EDGE& e) {}
  };

  void dfsImpl(int u, TraversalState& s)
  {
    s.discovered[u] = true;
    s.processEarly(u);

    for(const EDGE& e: adjacency[u]) {
      const int v = e.to;
      if(!s.discovered[v]) {
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
      for(const EDGE& e: adjacency[u]) {
        const int v = e.to;
        if(!s.processed[v] || DIRECTED) {
          s.processEdge(u,v);
        }
        if(!s.discovered[v]) {
          queue.push(v);
          s.discovered[v] = true;
          s.parent[v] = u;
        }
      }
      s.processLate(u);
    }
  }

protected:
  using EdgeList=std::vector<EDGE>;

  const size_t size;
  std::vector<EdgeList> adjacency;
};

} // namespace algo