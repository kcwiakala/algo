#include <array>
#include <bitset>
#include <Common.hpp>
#include <cstring>

namespace algo {
namespace rachel {

using Mask_t = std::bitset<32>;
constexpr size_t ALPHABET_SIZE = 'z' - 'a' + 1;
constexpr size_t MAX_LENGTH = 100000;

char text[MAX_LENGTH];
unsigned char maxDistinct;

struct Suffix
{
  Suffix(size_t b, size_t l): begin(b), length(l) 
  {
  }

  size_t begin;
  size_t length;
  int distinctPos[ALPHABET_SIZE] = {-1}; 

  void init()
  {
    int d=0;
    Mask_t mask;
    for(size_t i=0; i<length; ++i) {
      const size_t x = 'z' - text[begin+i];
      if(!mask.test(x)) {
        mask.set(x);
        distinctPos[d++] = i;
      }
      if(d == maxDistinct) {
        break;
      }
    }
    if(d < maxDistinct) {
      distinctPos[d] = length;
    }
  }

  int distinctAt(size_t len)
  {
    auto it = std::lower_bound(distinctPos, distinctPos+maxDistinct, len);
    return std::distance(distinctPos, it);
  }

  bool operator<(const Suffix& rhs)
  {
    if(length < rhs.length) {
      return std::strncmp(text+begin, text+rhs.begin, length) <= 0;
    } else {
      return std::strncmp(text+begin, text+rhs.begin, rhs.length) < 0;
    }
  }
};

class SuffixArray
{
public:
  SuffixArray(const std::string& s): size(s.size())
  {
    std::strncpy(text, s.c_str(), size);
    init();
  } 

  SuffixArray(size_t len): size(len)
  {
    init();
  } 

  void init()
  {
    suffixes.reserve(size);
    size_t length = size;
    Mask_t mask;
    for(size_t begin=0; begin<size; ++begin, --length) {
      suffixes.emplace_back(begin, length);
      mask.set('z' - text[begin]);
    }
    maxDistinct = mask.count();
    std::sort(suffixes.begin(), suffixes.end());
    for(Suffix& s: suffixes) {
      s.init();
    }

    lcp.assign(size, 0);
    for(size_t i=1; i<size; ++i) {
      auto end = text + suffixes[i-1].begin + std::min(suffixes[i-1].length, suffixes[i].length);
      auto m = std::mismatch(text + suffixes[i-1].begin, end, text+suffixes[i].begin);
      lcp[i] = std::distance(text + suffixes[i-1].begin, m.first);
    }

    count.assign(size, 0);
    count[0] = suffixes[0].length;
    for(size_t i=1; i<suffixes.size(); ++i) {
      count[i] = count[i-1] + suffixes[i].length - lcp[i];
    }
  }

  void print()
  {
    int counter = 0;
    for(size_t i=0; i<suffixes.size(); ++i) {
      const Suffix& sfx = suffixes[i];
      for(size_t len = lcp[i]+1; len <= sfx.length; ++len) {
        std::cout << ++counter << " : " << std::string(text+sfx.begin, len) << std::endl;
      }
    }
  }

  void traverse(unsigned long from, unsigned long to) {

  }

  std::string at(size_t idx)
  {
    auto it = std::lower_bound(count.cbegin(), count.cend(), idx);
    if(it == count.end()) {
      return std::string();
    } else {
      size_t i = std::distance(count.cbegin(), it);
      const Suffix& sfx = suffixes[i];
      int diff = *it-idx;
      std::string result(text + sfx.begin, sfx.length - diff);
      return result;
    }
  }

  int distinctAt(size_t idx)
  {
    auto it = std::lower_bound(count.cbegin(), count.cend(), idx);
    if(it == count.end()) {
      return -1;
    } else {
      size_t i = std::distance(count.cbegin(), it);
      Suffix& sfx = suffixes[i];
      return sfx.distinctAt(sfx.length - (*it-idx));
    }
  }

  const size_t size;
  std::vector<Suffix> suffixes;
  std::vector<size_t> lcp;
  std::vector<size_t> count;
};


TEST(RachelChallenge, test1) 
{
  const std::string s("banana");
  SuffixArray sa(s);
  //sa.print();
  //std::cout << sa.distinctAt(6) << std::endl;
  //std::cout << sa.distinctAt(11) << std::endl;

  // size_t count = sa.traverse();
  // for(size_t i=0; i<count; ++i) {
  //   std::cout << static_cast<int>(result[i]) << std::endl;
  // }

  // std::cout << "KUPA" << std::endl;
  for(size_t i=0; i<sa.count.back(); ++i) {
    std::cout << sa.distinctAt(i+1) << std::endl;
  }
}

} // namespace rachel
} // namespace algo