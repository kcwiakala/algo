#include <array>
#include <bitset>
#include <Common.hpp>
#include <cstring>
#include <random>

namespace algo {
namespace rachel {

using Mask_t = std::bitset<32>;
constexpr size_t ALPHABET_SIZE = 'z' - 'a' + 1;
constexpr size_t MAX_LENGTH = 100000;

char text[MAX_LENGTH];
size_t aux[MAX_LENGTH+1][ALPHABET_SIZE];
size_t suffixes[MAX_LENGTH];
size_t lcp[MAX_LENGTH];
size_t count[MAX_LENGTH];

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
    for(size_t i=0; i<size; ++i) {
      suffixes[i] = i;
    }
    auto suffixComparer = [size=size](size_t s1, size_t s2) {
      if(s1 < s2) { // 1. s1 longer than s2
        return std::strncmp(text+s1, text+s2, size-s2) < 0;
      } else { // 2. s1 shorter than s2
        return std::strncmp(text+s1, text+s2, size-s1) <= 0;
      }
    };
    std::sort(suffixes, suffixes+size, suffixComparer);

    for(size_t a=0; a<ALPHABET_SIZE; ++a) {
      aux[0][a] = 0;
    }
    for(size_t i=1; i<=size; ++i) {
      std::copy(aux[i-1], aux[i-1] + ALPHABET_SIZE, aux[i]);
      aux[i]['z' - text[i-1]] += 1;
    }
    
    lcp[0] = 0;
    for(size_t i=1; i<size; ++i) {
      const size_t minLength = size - std::max(suffixes[i-1], suffixes[i]);
      auto m = std::mismatch(text + suffixes[i-1], text + suffixes[i-1] + minLength, text+suffixes[i]);
      lcp[i] = std::distance(text + suffixes[i-1], m.first);
    }

    count[0] = (size - suffixes[0]);
    for(size_t i=1; i<size; ++i) {
      count[i] = count[i-1] + (size - suffixes[i]) - lcp[i];
    }
    total = count[size-1];
  }

  void print()
  {
    int counter = 0;
    for(size_t i=0; i<size; ++i) {
      const size_t length = size - suffixes[i];
      for(size_t len = lcp[i]+1; len <= length; ++len) {
        std::cout << ++counter << " : " << std::string(text+suffixes[i], len) << std::endl;
      }
    }
  }

  int distinct(size_t from, size_t to) 
  {
    int result = 0;
    for(size_t a=0; a<ALPHABET_SIZE; ++a) {
      if(aux[to][a] > aux[from][a]) {
        ++result;
      }
    }
    return result;
  }

  int distinctAt(size_t idx)
  {
    if(idx > total) {
      return -1;
    }
    auto it = std::lower_bound(count, count + size, idx);
    const size_t i = std::distance(count, it);
    const size_t diff = *it - idx;
    const size_t from = suffixes[i];
    const size_t to = size - diff;
    return distinct(from, to);
  }

  const size_t size;
  size_t total;
};

TEST(RachelChallenge, test1) 
{
  std::random_device randomDevice;
  std::default_random_engine randomEngine(randomDevice());
  std::uniform_int_distribution<char> gen('a', 'z');

  std::string longOne(100000, '\0');
  for(char& c: longOne) {
    c = gen(randomEngine); 
  }

  const std::string shortOne("banana");
  SuffixArray sa(longOne);
  // sa.print();
  std::cout << sa.total << std::endl;

  size_t queries = std::min(static_cast<size_t>(100000), sa.total);

  for(size_t i=1; i<=queries; ++i) {
    EXPECT_GT(sa.distinctAt(i), 0);
    // std::cout << sa.distinctAt(i) << std::endl;
  }
  //std::cout << sa.distinctAt(6) << std::endl;
  //std::cout << sa.distinctAt(11) << std::endl;

  // size_t count = sa.traverse();
  // for(size_t i=0; i<count; ++i) {
  //   std::cout << static_cast<int>(result[i]) << std::endl;
  // }

  // std::cout << "KUPA" << std::endl;
  // for(size_t i=0; i<sa.count.back(); ++i) {
  //   std::cout << sa.distinctAt(i+1) << std::endl;
  // }
}

} // namespace rachel
} // namespace algo