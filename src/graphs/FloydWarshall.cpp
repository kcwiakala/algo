#include <list>
#include <limits>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Graph.hpp>
#include <Matrix.hpp>

namespace algo {
namespace fw {

constexpr int MAX_INT = std::numeric_limits<int>::max();  

struct Graph: public GenericGraph<WeightedEdge>
{
  Graph(size_t s): GenericGraph(s) {}

  auto floydWarshall()
  {
    Matrix<int> D(size, size, MAX_INT);
    Matrix<int> P(size, size, -1);

    for(int i=0; i<size; ++i) {
      D(i,i) = 0;
      for(const EdgeType& e: adjacency[i]) {
        D(i,e.to) = e.weight;
        P(i,e.to) = i;
      }
    }

    for(int k=0; k<size; ++k) {
      for(int i=0; i<size; ++i) {
        for(int j=0; j<size; ++j) {
          if((D(i,k) != MAX_INT) && (D(k,j) != MAX_INT)) {
            int aux = D(i,k) + D(k,j);
            if(aux < D(i,j)) {
              D(i,j) = aux;
              P(i,j) = P(k,j);
            }
          }
        }
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

  auto result = g.floydWarshall();
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

} // namespace fw
} // namespace algo