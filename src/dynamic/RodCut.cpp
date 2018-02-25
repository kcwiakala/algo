#include <limits>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace algo {
namespace rod_cut {

void printSolution(const std::vector<int>& s, int n) 
{
  while(n > 0) {
    int length = s[n];
    n -= length;
    std::cout << length << (n>0 ? " + " : "");
  }
  std::cout << std::endl;
}

int maxPrice(const std::vector<int>& p, int n)
{
  std::vector<int> r(n+1);
  std::vector<int> s(n+1, 0);
  r[0] = 0;
  for(int j=1; j <= n ; ++j) {
    r[j] = std::numeric_limits<int>::min();
    for(int i=1; i <= j; ++i) {
      const int pi = (i-1 < p.size()) ? p[i-1] : 0; 
      int aux = pi + r[j-i];
      if(aux > r[j]) {
        s[j] = i;
        r[j] = aux;
      }
    }
  }
  printSolution(s, n);
  return r[n];
}

TEST(RodCut, test1)
{
  std::vector<int> p = {1,5,8,9,10,17,17,20,24,30};
  EXPECT_THAT(maxPrice(p, 7), testing::Eq(18));
  EXPECT_THAT(maxPrice(p, 9), testing::Eq(25));
  EXPECT_THAT(maxPrice(p, 30), testing::Eq(90));
}

} // namespace rod_cut
} // namespace algo