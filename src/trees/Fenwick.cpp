#include <Common.hpp>

namespace algo {

constexpr int LSB(int i) 
{
  return i & (-i);
}

template<typename T>
class FenwickTree
{
public:
  FenwickTree(const size_t N): tree(N, T(0)) {}

  template<typename Iterator>
  FenwickTree(Iterator begin, Iterator end): tree(std::distance(begin, end)+1, T(0))
  {
    int idx = 1;
    while(begin != end) {
      add(idx++, *(begin++));
    }
  }

  void add(size_t idx, const T& value)
  {
    while(idx < tree.size()) {
      tree[idx] += value;
      idx += LSB(idx);
    }
  } 

  T sum(size_t idx)
  {
    T s = 0;
    while(idx > 0) {
      s += tree[idx];
      idx -= LSB(idx);
    }
    return s;
  }

private:
  std::vector<T> tree;
};

TEST(Trees, Fenwick) 
{
  std::vector<int> a = {1,4,2,5,7,3,4,5};
  FenwickTree<int> fw(a.begin(), a.end());

  EXPECT_EQ(fw.sum(3), 7);
  EXPECT_EQ(fw.sum(4), 12);
  EXPECT_EQ(fw.sum(8), 31);

  fw.add(4,2);
  EXPECT_EQ(fw.sum(3), 7);
  EXPECT_EQ(fw.sum(4), 14);
  EXPECT_EQ(fw.sum(8), 33);
}

} // namespace algo