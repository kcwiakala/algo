#include <Common.hpp>

namespace algo {
namespace fh {

template<typename K>
struct Node
{
  Node(const K& v): value(v) {}

  using ptr_t = std::shared_ptr<Node>;
  using list_t = std::list<ptr_t>;

  ptr_t parent;
  list_t children;
  bool mark = false;
  K value;
};

template<typename K, typename Compare = std::less<K> >
class FibonacciHeap 
{
  using node_ptr_t = typename Node<K>::ptr_t;
  using node_list_t = typename Node<K>::list_t;
  using node_iter_t = typename node_list_t::iterator;

public:
  FibonacciHeap(): _min(_rootList.end()) 
  {
  }

  const K& min()
  {
    if(empty()) {
      throw std::runtime_error("Heap is empty");
    }
    return (*_min)->value;
  }

  const size_t& size() const
  {
    return _size;
  }

  bool empty() const
  {
    return (_min == _rootList.end());
  }

  K extract()
  {
    auto result = min();
    auto z = *_min;
    _rootList.erase(_min);
    for(auto child: z->children) {
      child->parent = nullptr;
    }
    _rootList.splice(_rootList.end(), z->children);
    if(_rootList.empty()) {
      _min = _rootList.end();
    } else {
      consolidate();
    }
    --_size;
    return result;
  }

  void insert(const K& k)
  {
    node_ptr_t node = std::make_shared<Node<K>>(k);
    _rootList.push_front(node);
    if(empty() || _cmp(k, min())) {
      _min = _rootList.begin(); 
    }
    ++_size;
  } 

  void unite(FibonacciHeap& rhs)
  {
    if(empty()) {
      _rootList.swap(rhs._rootList);
      std::swap(_min, rhs._min);
      std::swap(_size, rhs._size);
    } else if(!rhs.empty()) {
      auto lhsMin = *_min;
      auto rhsMin = *(rhs._min);
      if(_cmp(rhs.min(), min())) {
        _rootList.swap(rhs._rootList);
        std::swap(_min, rhs._min);
      }
      _rootList.splice(_rootList.end(), rhs._rootList);
      _size += rhs._size;
      rhs._size = 0;
      rhs._min = rhs._rootList.end();
    }
  }

private:
  void consolidate() 
  {
    std::vector<node_ptr_t> A(maxDegree(), nullptr);
    node_list_t aux;
    aux.swap(_rootList);
    for(auto w: aux) {
      auto x = w;
      size_t d = x->children.size();
      while(A[d] != nullptr) {
        auto y = A[d];
        if(_cmp(y->value, x->value)) {
          x.swap(y);
        }
        x->children.push_back(y);
        y->parent = x;
        y->mark = false;
        A[d] = nullptr;
        ++d;
      }
      A[d] = x;
    }
    _min = _rootList.end();
    for(auto node: A) {
      if(node != nullptr) {
        _rootList.push_front(node);
        if(empty() || _cmp(node->value, min())) {
          _min = _rootList.begin();
        }
      }
    }
  }

  int maxDegree() const
  {
    return static_cast<int>(std::log(_size) / std::log(1.62));
  }

private:
  Compare _cmp;
  node_list_t _rootList;
  node_iter_t _min;
  size_t _size = 0;
};


TEST(Heaps, FibonacciHeap_1)
{
  FibonacciHeap<int> heap;
  FibonacciHeap<int> beep;

  EXPECT_TRUE(heap.empty());
  heap.insert(5);
  heap.insert(3);
  heap.insert(2);
  heap.insert(8);
  heap.insert(12);

  EXPECT_THAT(heap.size(), testing::Eq(5));
  EXPECT_FALSE(heap.empty());
  EXPECT_THAT(heap.min(), testing::Eq(2));

  beep.insert(9);
  beep.insert(1);
  EXPECT_THAT(beep.size(), testing::Eq(2));
  
  heap.unite(beep);
  EXPECT_TRUE(beep.empty());
  EXPECT_THAT(beep.size(), testing::Eq(0));

  EXPECT_THAT(heap.size(), testing::Eq(7));
  EXPECT_THAT(heap.min(), testing::Eq(1));

  EXPECT_THAT(heap.extract(), testing::Eq(1));
  EXPECT_THAT(heap.size(), testing::Eq(6));
  EXPECT_THAT(heap.extract(), testing::Eq(2));
  EXPECT_THAT(heap.extract(), testing::Eq(3));
  EXPECT_THAT(heap.extract(), testing::Eq(5));
  EXPECT_THAT(heap.size(), testing::Eq(3));
}

} // namespace fh
} // namespace algo