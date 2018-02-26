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

TEST(Dynamic, LongestIncreasingSequence_1)
{
  sequence_t s = {11,2,4,3,5,1,7,6,9,8,1};
  auto result = longestIncreasingSequence(s);
  EXPECT_THAT(result.size(), testing::Eq(5));
  EXPECT_THAT(result, testing::ElementsAre(2,4,5,7,9));
}

} // namespace lis
} // namespace algo