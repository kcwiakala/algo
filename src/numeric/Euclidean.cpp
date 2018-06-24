#include <Common.hpp>

namespace algo {

int gcd(int x, int y) {
  if (x > y) {
    return gcd(y, x-y);
  } else if (x < y) {
    return gcd(x, y-x);
  } else {
    return x;
  }
}

int lcm(int x, int y) {
  return x*y / gcd(x,y);
}

TEST(Numeric, Euclidean) 
{
  EXPECT_EQ(gcd(252, 105), 21);
  EXPECT_EQ(lcm(28, 21), 84);
}

} // namespace algo