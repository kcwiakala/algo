#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Graph.hpp>
#include <Matrix.hpp>

namespace algo {
namespace hungarian {

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

struct BipartiteGraph
{
  using Flags=std::vector<bool>;

  BipartiteGraph(size_t l, size_t r): _M(l,r,-1) 
  {
  }

  void set(size_t i, const std::vector<int>& adj) 
  {
    for(int j=0; j<adj.size(); ++j) {
      _M(i,j) = adj[j];
    }
  }

  using Assignment = std::vector<int>;

  void assign(const Matrix<int>& M, Assignment& a)
  {
    const size_t H = M.height();
    const size_t W = M.width();
    const size_t rgSize = H + W + 2;

    // Create residual flow graph
    ResidualFlowGraph rg(rgSize);
    int source = rgSize-2;
    int sink = rgSize-1;
    for(int i=0; i<H; ++i) {
      for(int j=0;j<W;++j) {
        if(M(i,j) == 0) {
          rg.connect(i, j+H, 1);
          rg.connect(j+H, i, 0);
        }  
      }
      rg.connect(source, i, 1);
    }
    for(int j=0;j<W;++j) {
      rg.connect(j+H, sink, 1);
    }

    struct State: public ResidualFlowGraph::TraversalState
    {
      State(size_t s): ResidualFlowGraph::TraversalState(s) {}

      bool validEdge(const ResidualEdge& e)
      {
        return e.residual > 0;
      }
    };

    State state(rgSize);
    rg.bfsImpl(source, state);
    auto augmentingPath = rg.buildPath(source, sink, state.parent);
    // for(int i: augmentingPath) { std::cout << i << "->"; } std::cout << std::endl;

    while(!augmentingPath.empty()) {
      int augmentingVolume = rg.volume(augmentingPath);
      rg.augment(augmentingPath, augmentingVolume);
      state.reset();
      rg.bfsImpl(source, state);
      augmentingPath = rg.buildPath(source, sink, state.parent);
      // for(int i: augmentingPath) { std::cout << i << "->"; } std::cout << std::endl;
    }

    a.assign(H, -1);
    for(int i=0; i<H; ++i) {
      for(int j=0; j<W; ++j) {
        if(rg.flow(i, j+H) > 0) {
          a[i] = j;
          break;
        }
      }
    }
  }

  void crossOut(Matrix<int>& M, const Assignment& a)
  {
    const size_t H = M.height();
    const size_t W = M.width();

    Flags markedRows(H, false);
    Flags markedCols(W, false);

    Assignment aux = a;

    bool done = false;
    while(!done) {
      done = true;
      for(int i=0; i<H; ++i) {
        // Mark rows having no assignment
        if(aux[i] == -1) {
          aux[i] = -2;
          done = false;
          // std::cout << "Marking row " << i << std::endl;
          markedRows[i] = true;
          const auto& row = M.row(i);
          // Mark all (unmarked) columns having zeros in newly marked row
          for(int j=0; j<W; ++j) {
            if(row[j] == 0) {
              if(!markedCols[j]) {
                // std::cout << "Marking column " << j << std::endl;
                markedCols[j] = true;
                // Mark all rows having assignments in newly marked columns
                for(int k=0; k<H; ++k) {
                  if(aux[k] == j) {
                    aux[k] = -1;
                  }
                }
              }
            }
          }
        }
      }
    }

    // Cross out marked columns and unmarked rows
    for(int i=0; i<H; ++i) {
      markedRows[i] = !markedRows[i];
    }

    // From remaining elements find the minimum
    int minVal = std::numeric_limits<int>::max();
    for(int i=0; i<H; ++i) {
      if(markedRows[i]) {
        continue;
      }
      for(int j=0; j<W; ++j) {
        if(!markedCols[j] && M(i,j) < minVal) {
          minVal = M(i,j);
        }
      }
    }

    // Subtract min from every unmarked element and add it to every element covered by two lines.
    for(int i=0; i<H; ++i) {
      for(int j=0; j<W; ++j) {
        if(markedRows[i] && markedCols[j]) {
          M(i,j) += minVal;
        } else if(!markedRows[i] && !markedCols[j]) {
          M(i,j) -= minVal;
        }
      }
    }
  }

  auto perfectAssignment() 
  {
    std::vector<int> assignment(_M.height(), -1);

    Matrix<int> H = _M;
    
    // Step 1
    for(int i=0; i<H.height(); ++i) {
      const auto& row = H.row(i);
      int minVal = *std::min_element(row.cbegin(), row.cend());
      for(int j=0; j<H.width(); ++j) {
        H(i,j) -= minVal;
      }
    }
    // std::cout << H << std::endl;

    // Step 2
    for(int j=0; j<H.width(); ++j) {
      const auto& col = H.column(j);
      int minVal = *std::min_element(col.cbegin(), col.cend());
      for(int i=0; i<H.height(); ++i) {
        H(i,j) -= minVal;
      }
    }
    // std::cout << H << std::endl;

    // Step 3
    int steps = H.width();
    while(steps--) {
      assign(H, assignment);
      if(std::find(assignment.cbegin(), assignment.cend(), -1) == assignment.cend()) {
        break;
      }
      crossOut(H, assignment);
      // std::cout << H << std::endl;
    }

    return assignment;
  }

  int cost(const Assignment& a) 
  {
    int result = 0;
    for(int i=0; i<_M.height(); ++i) {
      result += _M(i, a[i]);
    }
    return result;
  }

  Matrix<int> _M;
};

TEST(Hungarian, test1) 
{
  // 3  5
  // 10 1
  BipartiteGraph g(2,2);
  g.set(0, {3,  5});
  g.set(1, {10, 1});

  auto a = g.perfectAssignment();
  EXPECT_THAT(a, testing::ElementsAre(0,1));
  EXPECT_THAT(g.cost(a), testing::Eq(4));
}

TEST(Hungarian, test2) 
{
  BipartiteGraph g(16, 16);
  g.set(0,  {19,22,22,79,31, 2,77,47, 8,28, 9,57,54,81,18, 8});
  g.set(1,  { 2,61,78,98,51,47,63,55, 7,93,27,59,49,24,56,27});
  g.set(2,  { 4,22,70,68,93,75,68,35,68,13,27,80,29,87, 9,72});
  g.set(3,  {36,87,60,76, 5,98, 5,37,50,29,52,73,18,17,77,95});
  g.set(4,  {87,68, 9, 9,29,94,93,28,25,65,62,50,73,77,22,92});
  g.set(5,  { 1,71,94,71,71,36,36,20,66,88,95,76,23,39,84,73});
  g.set(6,  {96,28,19,50,54,81,31,67,50, 2,34,65,22,77,16,51});
  g.set(7,  {100,24,30,17,27,45,54,60,14,43,29,6,50,66,80,43});
  g.set(8,  {43,93,23,52,13,54,7,87,95,18,70,100,40,77,40,30});
  g.set(9,  {53,16,60,68,19,48,88,37,73,86,69,10,13,74,26,84});
  g.set(10, {88, 9,14,18,51,38,44,52,27,34,39,40,95, 6,66,35});
  g.set(11, {97,29,49,16,57, 3,17,96,37,29,37,81,94,42,73,33});
  g.set(12, {75,34,31,65,44,25,20,19,68,21,48,19,83,96,57,37});
  g.set(13, {78,72,41,63,19,40,50,44,81, 4,61,22, 8,55,98,88});
  g.set(14, {29,52,51,87, 4,78,35,75,49,73,50,44,69,14,66,33});
  g.set(15, {33,37,11,95,80,88,82,46,97,62,14,13,67,33,97,47});


  auto a = g.perfectAssignment();
  BipartiteGraph::Assignment expected = {15,8,14,6,3,0,9,11,2,1,13,5,7,12,4,10};

  EXPECT_THAT(a, testing::ContainerEq(expected));
  EXPECT_THAT(g.cost(a), testing::Eq(140));
}

TEST(Hungarian, test3) 
{
  // 1 1 1
  // 9 1 8
  // 1 6 4
  BipartiteGraph g(3,3);
  g.set(0, {1, 1, 1});
  g.set(1, {9, 1, 8});
  g.set(2, {1, 6, 4});

  auto a = g.perfectAssignment();
  EXPECT_THAT(a, testing::ElementsAre(2,1,0));
  EXPECT_THAT(g.cost(a), testing::Eq(3));
}

} // namspace hungarian
} // namespace algo