#include <Common.hpp>

namespace algo {

std::vector<int> kmp(const std::string& text, const std::string& pattern)
{
  std::vector<int> result;
  const int P = pattern.size();
  if(P > text.size() || P == 0) {
    return result;
  }

  std::vector<int> T(P, 0);
  T[0] = -1;

  for(int i=1; i<P; ++i) {
    if(pattern[0] == pattern[i]) {
      T[i] = -1;
    }
    auto m = std::mismatch(pattern.begin(), pattern.end(), pattern.begin()+i, pattern.end());
    size_t l = std::distance(pattern.begin(), m.first);
    if(l > 0) {
      T[i+l] = l;
    }
  }
  // for(int t: T) {
  //   std::cout << t << " ";
  // }
  // std::cout << std::endl;

  int p = 0;
  int i = 0;
  while(i < text.size()) {
    if(text[i] == pattern[p]) {
      ++i;
      if(++p == P) {
        result.push_back(i-p);
        p=0;
      }
    } else {
      if(T[p] < 0) {
        ++i;
        p = 0;
      } else {
        p = T[p];
      }
    }
  }

  return result;
}

TEST(strings, KnuthMorrisPratt) 
{
  auto r1 = kmp("this is some very long and stupid text that is", "is");
  EXPECT_THAT(r1, testing::ElementsAre(2,5,44));

  auto r2 = kmp("ksjnffnsdmvkxchsbsndmfkcvxcjbsdnfbsdhsvjxhcsnfbmsd", "sndm");
  EXPECT_THAT(r2, testing::ElementsAre(17));

  auto r3 = kmp("abcbbaabacababcbaabbabbbabacababc", "abacababc");
  EXPECT_THAT(r3, testing::ElementsAre(6,24));
}

} // namespace algo