#include <Common.hpp>

namespace algo {
namespace tries {

class Node
{
public:
  using iterator_t = std::string::const_iterator;

  Node(char c): _c(c) {}

  ~Node()
  {
    for(Node* n: _children)
    {
      delete n;
    }
  }

  char get() const
  {
    return _c;
  }

  void insert(iterator_t begin, iterator_t end)
  {
    if(begin != end) {
      auto it = std::lower_bound(_children.begin(), _children.end(), *begin, [](Node* n, char c) {
        return n->get() < c;
      });
      if(it == _children.end() || (*it)->get() != *begin) {
        it = _children.insert(it, new Node(*begin));
      }
      (*it)->insert(++begin, end);
    }
  }

  bool contains(iterator_t begin, iterator_t end) const
  {
    if(begin != end) {
      auto it = std::lower_bound(_children.begin(), _children.end(), *begin, [](Node* n, char c) {
        return n->get() < c;
      });
      if(it != _children.end() && (*it)->get() == *begin) {
        return (*it)->contains(++begin, end);
      }
      return false;
    }
    return true;
  }

private:
  char _c;
  std::list<Node*> _children;
};

class Trie 
{
public:
  Trie(): _root(0) {}

  void insert(const std::string& s)
  {
    for(auto begin=s.begin(); begin != s.end(); ++begin) {
      _root.insert(begin, s.end());
    }
  }

  bool contains(const std::string& s) const
  {
    return _root.contains(s.begin(), s.end());
  }

private:
  Node _root;
};


TEST(Trees, trie_1)
{
  Trie t;
  t.insert("Hello");
  t.insert("Banana");
  t.insert("Help");

  EXPECT_TRUE(t.contains("Hello"));
  EXPECT_TRUE(t.contains("nana"));
  EXPECT_TRUE(t.contains("elp"));
  EXPECT_FALSE(t.contains("Alice"));
}

} // namespace tries
} // namespace algo