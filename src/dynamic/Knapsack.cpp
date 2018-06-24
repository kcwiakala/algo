#include <Common.hpp>

namespace algo {

using Item_t = std::tuple<int, int, int>; // SIZE, VALUE, COUNT
using ItemList_t = std::vector<Item_t>;

int knapsack(const ItemList_t& items, int N)
{
  const int K = items.size();

  int V[K+1][N+1];
  int C[K+1][N+1];

  for(int k=0; k<=K; ++k) {
    for(int i=0; i<=N; ++i) {
      V[k][i] = 0;
      C[k][i] = 0;
    }
  }

  for(int k=1; k<=K; ++k) {
    const int count = std::get<2>(items[k-1]);
    for(int u=1; u<=count; ++u) {
      const int size = u*std::get<0>(items[k-1]);
      const int value = u*std::get<1>(items[k-1]);
      for(int i=1; i<=N; ++i) {
        if(i < size) {
          V[k][i] = std::max(V[k][i], V[k-1][i]);
        } else {
          V[k][i] = std::max(std::max(V[k][i], V[k-1][i]), V[k-1][i-size] + value);
        }
      }
    }
  }

  for(int k=0; k<=K; ++k) {
    for(int i=0; i<=N; ++i) {
      std::cout << V[k][i] << " ";
    }
    std::cout << std::endl;
  }

  return V[K][N];
}

TEST(Dynamic, Knapsack) 
{
  ItemList_t items = {{2,5,3},{4,12,3},{7,20,2}};
  EXPECT_EQ(knapsack(items,29),81);

  // 7,7,4,4,4,2 = 40 + 36 + 5 = 81 
}

} // namespace algo