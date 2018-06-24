#include <Common.hpp>

namespace algo {

bool shuffle(const std::string& z, const std::string& x, const std::string& y) 
{
  const size_t X = x.size();
  const size_t Y = y.size();

  bool DP[X+1][Y+1];
  DP[0][0] = true;
  for(size_t i=1; i<=X; ++i) {
    DP[i][0] = DP[i-1][0] && (x[i-1] == z[i-1]);
  }
  for(size_t j=1; j<=Y; ++j) {
    DP[0][j] = DP[0][j-1] && (y[j-1] == z[j-1]);
  }

  for(size_t i=1; i<=X; ++i) {
    for(size_t j=1; j<=Y; ++j) {
      DP[i][j] = (DP[i-1][j] && (x[i-1] == z[i+j-1])) ||
                 (DP[i][j-1] && (y[j-1] == z[i+j-1]));
    }
  }
  for(size_t i=0; i<=X; ++i) {
    for(size_t j=0; j<=Y; ++j) {
      std::cout << DP[i][j] << " ";
    }
    std::cout << std::endl;
  }
  return DP[X][Y];
}

TEST(Dynamic, Shuffle) 
{
  EXPECT_TRUE(shuffle("cchocohilaptes", "chocolate", "chips"));
  EXPECT_FALSE(shuffle("chocochilatspe", "chocolate", "chips"));
}

} // namespace algo