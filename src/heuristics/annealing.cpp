#include <Common.hpp>
#include <random>

namespace algo {

namespace {
  std::random_device rd;
  std::mt19937 gen(rd());
}

template<size_t N>
using Matrix = int[N][N];

template<int N>
class Tsp
{
public: 
  using Path = std::array<int, N>;

  Tsp(const Matrix<N>& m)
  {
    for(int i=0; i<N; ++i) {
      for(int j=0; j<N; ++j) {
        g[i][j] = m[i][j];
      }
    }
  }

  int solution(int maxIter = 10000) 
  {
    std::uniform_real_distribution<> dis(0.0, 1.0);

    Path p;
    for(int i=0; i<N; ++i) {
      p[i] = i;
    }
    double temp = 1.0;

    for(int i=0; i<2*N; ++i) {
      auto t = transition(p);
      std::swap(p[t.first], p[t.second]);      
    }

    Path optimalPath = p;
    int minCost = cost(p);
    int counter = 0;
    while(maxIter--) {
      int oldCost = cost(p);
      auto t = transition(p);
      std::swap(p[t.first], p[t.second]);
      int newCost = cost(p);

      if(newCost > oldCost) {
        const double prob = std::exp(static_cast<double>(oldCost-newCost)/temp/10.0);
        if(prob < dis(gen)) {
          std::swap(p[t.first], p[t.second]);
        }
      }
      if(newCost < minCost) {
        minCost = newCost;
        optimalPath = p;
      }
      if(++counter == 100) {
        counter = 0;
        temp *= 0.99;
      }
    }

    return cost(optimalPath);
  }

  int cost(const Path& p) const {
    int c = 0;
    for(int i=0; i<N-1; ++i) {
      c += g[p[i]][p[i+1]];
    }
    return c + g[p.back()][p.front()];
  };

private:

  auto transition(Path& p) {
    static std::uniform_int_distribution<> dis(0, N-1);
    std::pair<int, int> r;
    do {
      r.first = dis(gen);
      r.second = dis(gen);
    } while(r.first == r.second);
    return r;
  }

private:
  Matrix<N> g;
};

TEST(annealing, tsp10) 
{
  Matrix<15> data = {{0,29,82,46,68,52,72,42,51,55,29,74,23,72,46},
                    {29,0,55,46,42,43,43,23,23,31,41,51,11,52,21},
                    {82,55,0,68,46,55,23,43,41,29,79,21,64,31,51},
                    {46,46,68,0,82,15,72,31,62,42,21,51,51,43,64},
                    {68,42,46,82,0,74,23,52,21,46,82,58,46,65,23},
                    {52,43,55,15,74,0,61,23,55,31,33,37,51,29,59},
                    {72,43,23,72,23,61,0,42,23,31,77,37,51,46,33},
                    {42,23,43,31,52,23,42,0,33,15,37,33,33,31,37},
                    {51,23,41,62,21,55,23,33,0,29,62,46,29,51,11},
                    {55,31,29,42,46,31,31,15,29,0,51,21,41,23,37},
                    {29,41,79,21,82,33,77,37,62,51,0,65,42,59,61},
                    {74,51,21,51,58,37,37,33,46,21,65,0,61,11,55},
                    {23,11,64,51,46,51,51,33,29,41,42,61,0,62,23},
                    {72,52,31,43,65,29,46,31,51,23,59,11,62,0,59},
                    {46,21,51,64,23,59,33,37,11,37,61,55,23,59,0}};

  Tsp<15> tsp(data);
  std::cout << tsp.solution() << std::endl;

  Tsp<15>::Path p = {1,13,2,15,9,5,7,3,12,14,10,8,6,4,11};
  for(int& v: p) { --v; }
  std::cout << tsp.cost(p) << std::endl;
}

} // namespace algo