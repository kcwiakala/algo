#include <memory>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace algo {
namespace rbt {

template<typename Key>
struct Node 
{
public:
  Node(const Key& k): _key(k)
  {
  }

  enum color_t {BLACK, RED};
  
  using ptr_t = std::shared_ptr<Node>;

  const Key _key;
  color_t _color;
  ptr_t _left;
  ptr_t _right;
  ptr_t _parent;
};

template<typename Key>
class Tree
{
public:
  using node_t = Node<Key>;
  using ptr_t = typename node_t::ptr_t;

  // Move y in place of x in tree structure
  void substitute(ptr_t x, ptr_t y)
  {
    y->_parent = x->_parent;
    if(x == x->_parent->_right) {
      x->_parent->_right = y;
    } else {
      x->_parent->_left = y;
    }
  }

// ========== ROTATION ==========
  void rotateLeft(ptr_t x)
  {
    auto y = x->_right;
    substitute(x, y);

    // Link x as y left child
    if(y->_left) {
      y->_left->_parent = x;
    }
    x->_right = y->_left;
    x->_parent = y;
    y->_left = x;
  }

  void rotateRight(ptr_t x)
  {
    auto y = x->_left;
    substitute(x, y);

    // Link x as y right child
    if(y->_right) {
      y->_right->_parent = x;
    }
    x->_left = y->_right;
    x->_parent = y;
    y->_right = x;
  }

// ========== INSERTION ==========
  bool insert(ptr_t z)
  {
    z->_left = _nil;
    z->_right = _nil;
    z->_color = node_t::RED;

    auto y = _nil;
    auto x = _root;

    while(x != _nil) {
      if(z->_key == x->_key) {
        return false;
      } 
      y = x;
      x = (z->_key < x->_key) ? x->_left : x->_right;
    }
    z->_parent = y;
    if(z->_parent == _nil) {
      _root = z;
    } else if (z->_key < y->_key) {
      y->_left = z;
    } else {
      y->_right = z;
    }
    return insertFixup(z);
  }

  bool insertFixup(ptr_t z)
  {

  }

private:
  ptr_t _root;
  ptr_t _nil;
};

} // namespace rbt
} // namespace algo