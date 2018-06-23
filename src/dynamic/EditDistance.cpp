#include <Common.hpp>

namespace algo {

int editDistance(const std::string& s1, const std::string& s2, int sub = 1, int ins = 1, int del = 1)
{
  const size_t N = s1.size();
  const size_t M = s2.size();
  enum Operation { KEEP, SUB, INS, DEL };
  int DP[N+1][M+1];
  Operation OP[N+1][M+1];
  DP[0][0] = 0;
  OP[0][0] = KEEP;
  for(int i=1; i<=N; ++i) {
    DP[i][0] = i*del;
    OP[i][0] = DEL;
  }
  for(int i=1; i<=M; ++i) {
    DP[0][i] = i*ins;
    OP[0][i] = INS;
  }
  for(int i=1; i<=N; ++i) {
    for(int j=1; j<=M; ++j) {
      DP[i][j] = DP[i-1][j-1];
      OP[i][j] = KEEP;
      if(s1[i-1] != s2[j-1]) {
        DP[i][j] = DP[i-1][j-1] + sub;
        OP[i][j] = SUB;
        const int delC = DP[i-1][j] + del;
        const int insC = DP[i][j-1] + ins;
        if(delC < DP[i][j]) {
          DP[i][j] = delC;
          OP[i][j] = DEL;
        }
        if (insC < DP[i][j]) {
          DP[i][j] = insC;
          OP[i][j] = INS;
        }
      }
    }
  }
  int i = N;
  int j = M;
  std::vector<std::string> operations;
  while(i>0 || j>0) {
    if(OP[i][j] == KEEP) {
      operations.push_back(std::string("keep '") + s1[--i] + "'");
      --j;
    } else if(OP[i][j] == SUB) {
      operations.push_back(std::string("substitute '") + s1[--i] + "' with '" + s2[--j] + "'");
    } else if(OP[i][j] == DEL) {
      operations.push_back(std::string("delete '") + s1[--i] + "'");
    } else if(OP[i][j] == INS) {
      operations.push_back(std::string("insert '") + s2[--j] + "'");
    }
  }
  std::reverse(operations.begin(), operations.end());
  // for(const auto& op: operations) {
  //   std::cout << op << std::endl;
  // }
  return DP[N][M];
}

TEST(Dynamic, EditDistance) 
{
  EXPECT_EQ(editDistance("you should not", "thou shalt not"), 5);

  EXPECT_EQ(editDistance("what the fuck", "what the hell"), 4);
}

} // namespace algo