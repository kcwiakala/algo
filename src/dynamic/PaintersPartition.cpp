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
  int M[K+1][N] = {{0}}; // Dynamic Programming Matrix
  Array B(N); // sum B[i] = A[0] + ... + A[i]
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


int partition(const Array& a, const size_t K)
{
  const size_t N = a.size();
  int DP[N][K];
  int PT[N][K];
  DP[0][0] = a[0];
  PT[0][0] = -1;
  for(size_t i=1; i<N; ++i) {
    DP[i][0] = DP[i-1][0] + a[i];
    PT[i][0] = -1;
  }

  for(size_t k=1;k<K;++k) {
    for(size_t i=1; i<N; ++i) {
      DP[i][k] = std::numeric_limits<int>::max();
      for(size_t j=1; j<=i; ++j) {
        int aux = std::max(DP[j-1][k-1], DP[i][0] - DP[j-1][0]);
        if(aux < DP[i][k]) {
          DP[i][k] = aux;
          PT[i][k] = j;
        }
      }
    }
  }
  std::vector<int> result;
  int k = K-1;
  int e = N-1;
  while(k>0) {
    int b = PT[e][k--];
    result.push_back(b);
    e = b-1; 
  }
  std::reverse(result.begin(), result.end());
  // for(int r: result) {
  //   std::cout << r << " ";
  // }
  // std::cout << std::endl;
  return DP[N-1][K-1];
}

TEST(Dynamic, BookPartiton)
{
  const Array A = {100, 200, 300, 400, 500, 600, 700, 800, 900};
  EXPECT_EQ(partition(A,3), 1700);
}

} // namespace painters_partition
} // namespace algo