#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <algorithm>
#include <memory>
#include <vector>
#include <queue>

namespace algo {
namespace btree {

template<typename Key> class Tree;

template<typename Key>
class Node 
{
public:
  using key_list_t = std::vector<Key>; 
  using ptr_t = std::shared_ptr<Node>;
  using children_list_t = std::vector<ptr_t>;

  friend class Tree<Key>;

  Node(size_t t): _t(t), _size(0), _leaf(true) {}

  size_t size() const
  {
    return _keys.size();
  }

  bool search(const Key& k) 
  {
    auto it = std::lower_bound(_keys.begin(), _keys.end(), k);
    if(it != _keys.end() && *it == k) {
      return true;
    } else if (_leaf) {
      return false;
    } else {
      const size_t i = std::distance(_keys.begin(), it);
      // DISK_READ(child, i);
      auto child = *std::next(_children.begin(), i);
      return child->search(k);
    }
  }

  void remove(const Key& k) 
  {
    auto kit = std::lower_bound(_keys.begin(), _keys.end(), k);
    if(kit != _keys.end() && *kit == k) {
      if(_leaf) {
        _keys.erase(kit);
      } else {
        const size_t i = std::distance(_keys.begin(), kit);
        auto y = *std::next(_children.begin(), i);
        auto z = *std::next(_children.begin(), i+1);
        if(y->size() >= _t) {
          *kit = y->back();
          y->remove(*kit);
        } else if(z->size() >= _t) {
          *kit = z->front();
          z->remove(*kit);
        } else {
          // merge k and z into y
          y->_keys.push_back(*kit);
          y->_keys.insert(y->_keys.end(), z->_keys.begin(), z->_keys.end());
          if(!y->_leaf) {
            y->_children.insert(y->_children.end(), z->_children.begin(), z->_children.end());
          }
          _keys.erase(kit);
          _children.erase(std::next(_children.begin(), i+1));
          y->remove(k);
        }
      }
    } else {
      const size_t i = std::distance(_keys.begin(), kit);
      auto child = *std::next(_children.begin(), i);
      if(child->size() < _t) {
        ptr_t left((i > 0) ? *std::next(_children.begin(), i-1) : ptr_t());
        ptr_t right((i < _keys.size()) ? *std::next(_children.begin(), i+1) : ptr_t());
        if(left && left->size() >= _t) {
          --kit;
          child->_keys.insert(child->_keys.begin(), *kit);
          *kit = left->_keys.back();
          left->_keys.pop_back();
          if(!child->_leaf) {
            child->_children.insert(child->_children.begin(), left->_children.back());
            left->_children.pop_back();
          }
        } else if(right && right->size() >= _t) {
          child->_keys.push_back(*kit);
          *kit = right->_keys.front();
          right->_keys.erase(right->_keys.begin());
          if(!child->_leaf) {
            child->_children.push_back(right->_children.front());
            right->_children.erase(right->_children.begin());
          }
        } else if (left) {
          --kit;
          left->_keys.push_back(*kit);
          left->_keys.insert(left->_keys.end(), child->_keys.begin(), child->_keys.end());
          if(!child->_leaf) {
            left->_children.insert(left->_children.end(), child->_children.begin(), child->_children.end());
          }
          _keys.erase(kit);
          _children.erase(std::next(_children.begin(), i));
          child = left;
        } else if (right) {
          child->_keys.push_back(*kit);
          child->_keys.insert(child->_keys.end(), right->_keys.begin(), right->_keys.end());
          if(!child->_leaf) {
            child->_children.insert(child->_children.end(), right->_children.begin(), right->_children.end());
          }
          _keys.erase(kit);
          _children.erase(std::next(_children.begin(), i+1));
        }
      }
      child->remove(k);
    }
  }

  bool insertNonFull(const Key& k)
  {
    auto kit = std::lower_bound(_keys.begin(), _keys.end(), k);
    if(kit != _keys.end() && *kit == k) {
      // key duplicate
      return false;
    }
    if(_leaf) {
      _keys.insert(kit, k);
    } else {
      const size_t i = std::distance(_keys.begin(), kit);
      // DISK_READ(child, i);
      auto child = *std::next(_children.begin(), i);
      if(child->size() == 2*_t - 1) {
        splitChild(i);
        return insertNonFull(k);
      }
      return child->insertNonFull(k);
    }
  }

  void print() 
  {
    std::cout << (_leaf ? "LEAF NODE: " : "INTERNAL NODE: "); 
    for(auto k: _keys) {
      std::cout << k << " ";
    }
    std::cout << std::endl;
  }

private:
  const Key& back() const
  {
    if(_leaf) {
      return _keys.back();
    } else {
      return _children.back()->back();
    }
  }

  const Key& front() const
  {
    if(_leaf) {
      return _keys.front();
    } else {
      return _children.front()->front();
    }
  }

  void splitChild(size_t i)
  {
    // Child node to be splitted
    auto y = _children[i]; 

    // Mid key to be moved to parent
    Key mid = y->_keys[_t-1]; 

    // Copy upper part of y to new node z
    auto z = std::make_shared<Node>(_t);
    z->_leaf = y->_leaf;
    z->_keys.reserve(_t-1);
    std::copy(std::next(y->_keys.begin(), _t), y->_keys.end(), std::back_inserter(z->_keys));
    y->_keys.resize(_t-1);
    if(!y->_leaf) {
      z->_children.reserve(_t);
      std::copy(std::next(y->_children.begin(), _t), y->_children.end(), std::back_inserter(z->_children));
      y->_children.resize(_t);
    }

    // Insert mid key and new child node
    auto kit = std::next(_keys.begin(), i); 
    auto cit = std::next(_children.begin(), i+1);
    _keys.insert(kit, mid);
    _children.insert(cit, z);
  }

private:
  const size_t _t;
  size_t _size;
  key_list_t _keys;
  bool _leaf;
  children_list_t _children;
};

template<typename Key>
class Tree 
{
public:
  using node_t = Node<Key>;

  Tree(size_t t): _t(t), _root(std::make_shared<node_t>(t)) 
  {
    // DISK_WRITE(_root);
  }

  bool search(const Key& k)
  {
    return _root->search(k);
  }

  bool insert(const Key& k)
  {
    if(_root->size() == 2*_t - 1) {
      auto s = std::make_shared<node_t>(_t);
      s->_children.push_back(_root);
      s->_leaf = false;
      _root = s;
      _root->splitChild(0);
    }
    return _root->insertNonFull(k);
  }

  bool remove(const Key& k)
  {
    if(_root->search(k)) {
      _root->remove(k);
      return true;
    } else {
      return false;
    }
  }

  void print() 
  {
    std::cout << ">>>>" << std::endl;
    std::queue<typename node_t::ptr_t> q;
    q.push(_root);
    while(!q.empty()) {
      auto node = q.front();
      q.pop();
      node->print();
      for(auto c: node->_children) {
        q.push(c);
      }
    }
    std::cout << "<<<<" << std::endl;
  }

private:
  const size_t _t; // tree width factor;
  typename node_t::ptr_t _root;
};

TEST(BTree, test1)
{
  Tree<char> t(3);

  EXPECT_FALSE(t.search('a'));

  EXPECT_TRUE(t.insert('a'));
  EXPECT_TRUE(t.search('a'));

  EXPECT_FALSE(t.insert('a'));
  EXPECT_TRUE(t.insert('x'));
  EXPECT_TRUE(t.insert('d'));
  EXPECT_TRUE(t.insert('e'));
  EXPECT_TRUE(t.insert('q'));
  EXPECT_TRUE(t.insert('o'));
  EXPECT_TRUE(t.insert('i'));
  EXPECT_TRUE(t.insert('u'));
  EXPECT_TRUE(t.insert('m'));

  EXPECT_TRUE(t.insert('b'));

  t.print();

  EXPECT_FALSE(t.insert('i'));

  EXPECT_TRUE(t.remove('u'));
  t.print();
  EXPECT_TRUE(t.remove('x'));
  t.print();
  EXPECT_TRUE(t.remove('e'));

  t.print();
  EXPECT_TRUE(t.remove('d'));
  // EXPECT_FALSE(t.remove('s'));
  t.print();
}

} // namespace btree
} // namespace algo