#include <Common.hpp>

namespace algo {
namespace lcs {

std::string longestCommonSubsequence(const std::string& x, const std::string& y)
{
  const size_t N = x.size();
  const size_t M = y.size();

  int L[N][M] = {{0}};
  int S[N][M] = {{0}};
  for(int i=0; i<N; ++i) {
    L[i][0] = (x[i] == y[0]) ? 1 : 0;
  }
  for(int j=0; j<M; ++j) {
    L[0][j] = (x[0] == y[j]) ? 1 : 0;
  }

  for(int i=1; i<N; ++i) {
    for(int j=1; j<M; ++j) {
      if(x[i] == y[j]) {
        L[i][j] = L[i-1][j-1] + 1;
        S[i][j] = 1;
      } else if (L[i-1][j] > L[i][j-1]) {
        L[i][j] = L[i-1][j];
        S[i][j] = 2;
      } else {
        L[i][j] = L[i][j-1];
        S[i][j] = 3;
      }
    }
  }

  // Construct the solution
  std::string lcs;
  int i=N-1;
  int j=M-1;
  while(i >= 0 && j >= 0) {
    if(S[i][j] == 1) {
      lcs.push_back(x[i]);
      i -= 1;
      j -= 1;
    } else if(S[i][j] == 2) {
      i -= 1;
    } else if(S[i][j] == 3) {
      j -= 1;
    } else {
      lcs.push_back(x[i]);
      break;
    }
  }
  std::reverse(lcs.begin(), lcs.end());

  return lcs;
}

TEST(Dynamic, LongestCommonSubsequence_1)
{
  std::string x("ABCBDAB");
  std::string y("BDCABA");
  auto lcs = longestCommonSubsequence(x,y);
  EXPECT_THAT(lcs, testing::Eq("BDAB"));
}

} // namespace lcs
} // namespace algo