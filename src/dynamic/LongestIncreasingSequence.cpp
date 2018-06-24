#include <Common.hpp>

namespace algo {
namespace lis {

using sequence_t = std::vector<int>;

sequence_t longestIncreasingSequence(const sequence_t& s)
{
  sequence_t l(s.size(), 1);
  sequence_t prev(s.size(), -1);
  prev[0] = 0;
  for(int i=1; i<l.size(); ++i) {
    for(int j=0; j<i; ++j) {
      if((s[i] > s[j]) && (l[j] >= l[i])) {
        l[i] = l[j] + 1;
        prev[i] = j;
      }
    }
  }
  size_t iLast = std::distance(l.begin(), std::max_element(l.begin(), l.end()));
  sequence_t solution;
  solution.push_back(s[iLast]);
  int length = l[iLast];
  while(--length) {
    iLast = prev[iLast];
    solution.push_back(s[iLast]);
  }
  std::reverse(solution.begin(), solution.end());
  return solution;
}

sequence_t longestIncreasingSequence2(const sequence_t& s1)
{
  sequence_t s2 = s1;
  std::sort(s2.begin(), s2.end());
  const size_t N = s1.size();
  int DP[N][N];
  int S[N][N];
  for(int i=0; i<N; ++i) {
    DP[0][i] = s1[0] == s2[i];
    DP[i][0] = s1[i] == s2[0];
    S[0][i] = S[i][0] = 0;
  }
  for(int i=1;i<N;++i) {
    for(int j=1;j<N;++j) {
      if(s1[i] == s2[j]) {
        DP[i][j] = DP[i-1][j-1] + 1;
        S[i][j] = 0;
      } else if (DP[i-1][j] > DP[i][j-1]) {
        DP[i][j] = DP[i-1][j];
        S[i][j] = -1;
      } else {
        DP[i][j] = DP[i][j-1];
        S[i][j] = 1;
      }
    }
  }
  
  int i=N-1;
  int j=N-1;
  sequence_t solution;
  while(i>0 && j>0) {
    if(S[i][j] == 0) {
      solution.push_back(s1[i--]);
      --j;
    } else if (S[i][j] < 0) {
      --i;
    } else {
      --j;
    }
  }
  std::reverse(solution.begin(), solution.end());
  return solution;
}

TEST(Dynamic, LongestIncreasingSequence_1)
{
  sequence_t s = {11,2,4,3,5,1,7,6,9,8,1};
  auto result = longestIncreasingSequence(s);
  EXPECT_THAT(result.size(), testing::Eq(5));
  EXPECT_THAT(result, testing::ElementsAre(2,4,5,7,9));

  result = longestIncreasingSequence2(s);
  EXPECT_THAT(result.size(), testing::Eq(5));
  EXPECT_THAT(result, testing::ElementsAre(2,3,5,6,8));
}

} // namespace lis
} // namespace algo