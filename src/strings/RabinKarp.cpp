#include <Common.hpp>

namespace algo {

std::vector<int> rabinKarp(const std::string& text, const std::string& pattern)
{
  const int Q = 2;
  const int M = 17;

  std::vector<int> result;
  const int P = pattern.size();
  if(P > text.size()) {
    return result;
  }
  int ph = 0;
  int th = 0;
  int h = 1;
  for(int i=1; i<P; ++i) {
    h = (h*Q) % M;
  }
  for(int i=0; i<P; ++i) {
    ph = (ph * Q + pattern[i]) % M;
    th = (th * Q + text[i]) % M;
  }

  // std::cout << "ph " << ph << std::endl;

  for(int i=0; i<text.size() - P + 1; ++i) {
    // std::cout << "th[" << i << "]: " << th << std::endl;
    if(th == ph) {
      if(std::equal(pattern.begin(), pattern.end(), text.begin()+i)) {
        result.push_back(i);
      }
    }
    th = (th - h*text[i]) % M;
    th = (Q * th + text[i+P]) % M;

  }

  return result;
}

TEST(strings, RabinKarp) 
{
  auto rk1 = rabinKarp("this is some very long and stupid text that is", "is");
  EXPECT_THAT(rk1, testing::ElementsAre(2,5,44));

  auto rk2 = rabinKarp("ksjnffnsdmvkxchsbsndmfkcvxcjbsdnfbsdhsvjxhcsnfbmsd", "sndm");
  EXPECT_THAT(rk2, testing::ElementsAre(17));

  auto rk3 = rabinKarp("abcbbaabacababcbaabbabbbabacababc", "abacababc");
  EXPECT_THAT(rk3, testing::ElementsAre(6,24));
}

} // namespace algo