#include <algorithm>
#include <limits>
#include <numeric>
#include <vector>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace algo {
namespace painters_partition {

using Array = std::vector<int>;
constexpr int INF = std::numeric_limits<int>::max();

int minTime(const Array& A, int K) 
{
  const int N = A.size();
  int M[K+1][N] = {{0}};
  Array B(N);
  for(int i=0; i<N; ++i) {
    M[0][i] = INF;
    B[i] = A[i] + ((i > 0) ? B[i-1] : 0);
    M[1][i] = B[i];
  }

  for(int k=2; k <= K; ++k) {
    for(int n=0; n<N; ++n) {
      if(n < k-1) {
        M[k][n] = M[k-1][n];
        continue;
      }
      int minSoFar = M[k-1][n];
      for(int p=0; p<n; ++p) {
        int sumRight = B[n] - B[p];
        int aux = std::max(M[k-1][p], sumRight);
        minSoFar = std::min(minSoFar, aux);
      }
      M[k][n] = minSoFar;
    }
  }

  return M[K][N-1];
}

TEST(Dynamic, PaintersPartition_1) 
{
  const Array A = {10,10,10,10};
  const int K = 2;
  EXPECT_THAT(minTime(A,K), testing::Eq(20));
}

TEST(Dynamic, PaintersPartition_2) 
{
  const Array A = {10,20,30,40};
  const int K = 2;
  EXPECT_THAT(minTime(A,K), testing::Eq(60));
}

} // namespace painters_partition
} // namespace algo