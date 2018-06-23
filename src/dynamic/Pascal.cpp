#include <Common.hpp>

namespace algo {

TEST(Dynamic, pascal) 
{
  const size_t N = 101;
  long P[N][N];

  // initialization
  for(int n=0; n<N; ++n) {
    P[n][0] = 1;
    P[n][1] = n;
    P[n][n] = 1;
  }

  // Dynamic Programming
  for(int n=1; n<N; ++n) {
    for(int k=2; k<n; ++k) {
      P[n][k] = P[n-1][k-1] + P[n-1][k];
    }
  }

  EXPECT_EQ(P[2][1], 2);
  EXPECT_EQ(P[12][5], 792);
  EXPECT_EQ(P[16][6], 8008);
}

} // namespace algo