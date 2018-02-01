#include <iostream>
#include <limits>
#include <vector>

#include <gtest/gtest.h>

namespace algo {

template<typename T>
using array_t = std::vector<T>;

template<typename T>
using array_2d_t = array_t<array_t<T>>;

using matrix_chain_t = std::vector<std::pair<unsigned, unsigned> >;

unsigned cost(const matrix_chain_t& chain, size_t i, size_t k, size_t j)
{
  const unsigned c = chain[i].first * chain[k].first * chain[j].second;
  std::cout << "Cost of " << i << " " << k << " " << j << " is " << c << std::endl;
  return c;
}

template<typename T>
void init(array_2d_t<T>& m, size_t n, T&& val)
{
  m.resize(n, array_t<T>(n, std::forward<T>(val)));
}

void solution(std::ostream& strm, const array_2d_t<int>& s, size_t i, size_t j)
{
  if(i == j) 
  {
    strm << "A" << i;
  }
  else 
  {
    strm << "(";
    solution(strm, s, i, s[i][j] - 1);
    solution(strm, s, s[i][j], j);
    strm << ")";
  }
}

TEST(MatrixMultiplication, basic)
{
  matrix_chain_t chain = {{30,35}, {35, 15}, {15, 5}, {5, 10}, {10, 20}, {20, 25}};

  const size_t N = chain.size();

  // Initialize cost array
  array_2d_t<unsigned> m;
  init(m, N, std::numeric_limits<unsigned>::max());
  for(size_t i=0; i<N; ++i) 
  {
    m[i][i] = 0;
  }

  array_2d_t<int> s;
  init(s, N, -1);

  for(size_t d=1; d<N; ++d)
  {
    for(size_t i=0; i<N-d; ++i)
    {
      size_t j=i+d;
      std::cout << "Checking m[" << i << "][" << j << "]" << std::endl; 
      for(size_t k=i+1; k<=j; ++k)
      { 
        unsigned c = m[i][k-1] + m[k][j] + cost(chain,i,k,j);
        if(c < m[i][j])
        {
          m[i][j] = c;
          s[i][j] = k;
        }
      }
    }
  }

  std::cout << "Total cost: " << m[0][N-1] << std::endl;

  for(size_t j=0; j<N; ++j)
  {
    for(size_t i=0; i<N; ++i)
    {
      std::cout << m[i][j] << " ";
    } 
    std::cout << std::endl;
  }
  solution(std::cout, s, 0, N-1);
  std::cout << std::endl;
}

} // namespace algo