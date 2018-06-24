#include <Common.hpp>

namespace algo {

std::vector<int> kadane(const std::vector<int>& a) 
{
  std::vector<int> subarray;
  if(a.empty()) {
    return subarray;
  }

  int maxSoFar = a[0];
  int maxHere = maxSoFar;
  int lastIdx = 0;
  for(int i=1; i<a.size(); ++i) {
    maxHere = std::max(maxHere + a[i], a[i]);
    if(maxHere > maxSoFar) {
      maxSoFar = maxHere;
      lastIdx = i;
    }
  }

  int sum = 0;
  while(sum < maxSoFar) {
    subarray.push_back(a[lastIdx--]);
    sum += subarray.back();
  }
  std::reverse(subarray.begin(), subarray.end());

  return subarray;
}

TEST(Dynamic, Kadane) 
{
  auto r = kadane({1,2,3,-4,-2,5,2,5,-3,6,-7,2});
  EXPECT_THAT(r, testing::ElementsAre(5,2,5,-3,6));
}

} // namespace algo