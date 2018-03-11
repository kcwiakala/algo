#include <Common.hpp>
#include <bitset>
#include <fstream>

namespace algo {
namespace ukkonen {

std::ofstream f("output.txt");

struct Result
{
  Result(unsigned long start, unsigned char len): startIdx(start), length(len) {}

  unsigned long startIdx;
  unsigned char length;

  bool operator<(unsigned long idx) const 
  {
    return startIdx < idx;
  }
};

using ResultList_t = std::vector<Result>;
using Mask_t = std::bitset<32>;

size_t idx(char c)
{
  return c - 'a';
}

template<size_t ALPHABET_SIZE>
class Node
{
public:
  Node(size_t s, size_t e = std::numeric_limits<size_t>::max()): start(s), end(e) {}

  void traverse(const char* text, unsigned long& counter, ResultList_t& result, Mask_t mask)
  {
    size_t pos = start;
    size_t distinct = mask.count();
    while(pos <= end && text[pos] != 0) {
      ++counter;
      const size_t cidx = idx(text[pos]);
      if(!mask.test(cidx)) {
        ++distinct;
        mask.set(cidx);
        result.emplace_back(counter, distinct);
      } 
      //std::cout << counter << " " << mask.count() << std::endl;
      ++pos;
    }

    for(Node* n: next) {
      if(n != nullptr) {
        n->traverse(text, counter, result, mask);
      }
    }
  }

  void print(std::string prefix, const char* text, unsigned long& counter) 
  {
    size_t pos = start + 1;
    while(pos < end && text[pos] != 0) {
      ++counter;
      f << counter << ": " << prefix << std::string(text+start, text+pos) << std::endl;
      ++pos;
    }
    prefix.append(text+start, text+pos); 

    for(Node* n: next) {
      if(n != nullptr) {
        n->print(prefix, text, counter);
      }
    }
  }

  size_t start;
  size_t end;
  Node* suffixLink = nullptr;

  std::array<Node*, ALPHABET_SIZE> next = {{nullptr}};
};

template<size_t MAX_LENGTH, size_t ALPHABET_SIZE>
class SuffixTree
{
public:
  using Node_t = Node<ALPHABET_SIZE>;

  SuffixTree()
  {
    std::fill(_text, _text+MAX_LENGTH, 0);

    _root = new Node_t(0);
    _activeNode = _root;
  }

  void extend(const std::string& str)
  {
    for(char c: str) {
      extend(c);
    }
    std::cout << _text << std::endl;
  }

  void extend(char c)
  {
    _text[++_pos] = c;
    _needsSuffixLink = nullptr;
    ++_remainder;

    while(_remainder > 0) {
      if(_activeLength == 0) {
        _activeEdge = _pos;
      }
      Node_t* next = _activeNode->next[activeEdgeIdx()];
      if(next == nullptr) {
        Node_t* leaf = new Node_t(_pos);
        _activeNode->next[activeEdgeIdx()] = leaf;
        addSuffixLink(leaf);
      } else {
        if(walkDown(next)) {
          continue;
        }
        if(_text[next->start + _activeLength] == c) {
          ++_activeLength;
          addSuffixLink(_activeNode);
          break;
        }
        Node_t* split = new Node_t(next->start, next->start + _activeLength);
        Node_t* leaf = new Node_t(_pos);
        _activeNode->next[activeEdgeIdx()] = split;
        split->next[edgeIdx(_pos)] = leaf;
        next->start += _activeLength;
        split->next[edgeIdx(next->start)] = next;
        addSuffixLink(split);
      }
      --_remainder;
      if(_activeNode == _root && _activeLength > 0) {
        --_activeLength;
        _activeEdge = _pos - _remainder + 1;
      } else {
        _activeNode = (_activeNode->suffixLink != nullptr) ? _activeNode->suffixLink : _root;
      }
    }
  }

  unsigned long traverse(ResultList_t& result)
  {
    unsigned long counter = 0;
    Mask_t mask;
    for(Node_t* n: _root->next) {
      if(n != nullptr) {
        n->traverse(_text, counter, result, mask);
      }
    }
    return counter;
  }

  void print()
  {
    unsigned long counter = 0;
    for(Node_t* n: _root->next) {
      if(n != nullptr) {
        std::cout << "Going to node " << _text[n->start] << std::endl;
        n->print("", _text, counter);
      }
    }
  }

private:
  void addSuffixLink(Node_t* node)
  {
    if(_needsSuffixLink != nullptr) {
      _needsSuffixLink->suffixLink = node;
    }
    _needsSuffixLink = node;
  }

  bool walkDown(Node_t* node)
  {
    const size_t nodeLength = std::min(static_cast<int>(node->end), _pos+1) - node->start;
    if(_activeLength >= nodeLength) {
      _activeEdge += nodeLength;
      _activeLength -= nodeLength;
      _activeNode = node;
      return true;
    }
    return false;
  }

  size_t activeEdgeIdx() const
  {
    return edgeIdx(_activeEdge);
  }

  size_t edgeIdx(size_t edge) const
  {
    return _text[edge] - 'a';
  }

private:
  char _text[MAX_LENGTH];
  int _pos = -1;

  Node_t* _root;
  Node_t* _activeNode;

  size_t _activeEdge = 0;
  size_t _activeLength = 0;

  Node_t* _needsSuffixLink = nullptr;
  size_t _remainder = 0;
};

int answer(const ResultList_t& result, unsigned long query) 
{
  auto it = std::lower_bound(result.cbegin(), result.cend(), query);
  if(it == result.cend()) {
    return result.back().length;
  } else {
    return it->length;
  }
}

TEST(Ukkonen, test1)
{
  SuffixTree<1000, 26> st;
  // st.extend("banana");
  st.extend("kfjskjfflsdfnsdfksjdfksdjfdsbfmdnsbfmsndbfmnsdbfmnsdbfmnsdbfmnsdbmfbsdkjhvkxjchvkxchiuvhkjdfnksjdhfkuhvjsdbfkjsdhfuyiuwyrjbfnsdkfjsdkjfbsdfsdmnfbsdmfjhsdgfzxbcmzfkjshfkjdshfkjdsfmspoasidkrwenmsdbfmsdbbfjhvxcbmvhsdjfhgsjdhgjgxgqhjhbndmfnsbdmfnbsdmfnbsdmfnbmsdnbfkjhvkxcjhvkjsdhkfsdhkjvxknsmbfmsdnfbmsdjxcvhdfnbvsdnfbvsdnfvjxchgvjdsfnsdbfvnsdbvfnsdhgvjhxcvsdbnfbvsdnfvbsndbvfnsbdsamdfnbdsmfbjhvjsdhfkjsdhkfjbdsnfsdnfvsdnbvfnsdbvfndsbvfndsbvfnsdjfhksdjhfksdjhfksjdhfsdvfnsdvnfvbsdbnfvdsniodasudkwqhmenbmfnbdsjhjhcvmbsnadsandasndbasbdmashdkjashdasdnbasvndbavsndasdhkasjhkjchzkjchkdmasnbdanmsddkjasndkjasnkhczuxasdasdasnbdmasnbdmasnbdmasnbdmasbdmasbdmsabdmnasbdmnasbdmansbdmasnbdmanskczhuasddasdagfsdiufydskjfdbfmsndbfmsndbkjhcvkjxchkvjsdbfmnsdbmfnbsdmnfbmdsfbmsdnbfmsdnbfmnsdbfmnsdbfmnsdbfmnsbdkjhxkcvhxdkasjhdsadnmasbdmnasbdmbasjkdhaksjdhkasjhdksahdkjashdkashdkjahskdhaskjdhkasjdhkasjhdkasjhdkasjhdkajshdkjashd");

  st.print();
  // ResultList_t result;
  // auto total = st.traverse(result);
  // for(auto r: result) {
  //   std::cout << r.startIdx << " / " << static_cast<int>(r.length) << std::endl;
  // }
}

} // namespace ukkonen
} // namespace algo 