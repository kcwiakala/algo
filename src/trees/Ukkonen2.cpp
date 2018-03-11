#include <Common.hpp>
#include <bitset>
#include <fstream>

namespace algo {
namespace ukkonen2 {

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

const int oo = 1<<25;
const int ALPHABET_SIZE = 'z' - 'a' + 1;
const int MAXN = 5000;

using namespace std;

int root, last_added, pos, needSL, remainder,
	active_node, active_e, active_len;

struct node {
/*
   There is no need to create an "Edge" struct.
   Information about the edge is stored right in the node.
   [start; end) interval specifies the edge,
   by which the node is connected to its parent node.
*/

	int start, end, slink;
	int next[ALPHABET_SIZE];	

	int edge_length() {
		return min(end, pos + 1) - start;
	}
};

char text[MAXN];
node tree[2*MAXN];

size_t idx(char c)
{
  return c - 'a';
}

void traverse(const node& n, unsigned long& counter, ResultList_t& result, Mask_t mask)
{
  size_t p = n.start;
  size_t distinct = mask.count();
  while(p>=0 && p <= n.end && text[p] != 0) {
    ++counter;
    const size_t cidx = idx(text[p]);
    if(!mask.test(cidx)) {
      ++distinct;
      mask.set(cidx);
      result.emplace_back(counter, distinct);
    } 
    ++p;
  }
  for(int i=0; i<ALPHABET_SIZE; ++i) {
    if(n.next[i] > 0) {
      traverse(tree[n.next[i]], counter, result, mask);
    }
  }
}

int new_node(int start, int end = oo) {
	node nd;
	nd.start = start;
	nd.end = end;
	nd.slink = 0;
	for (int i = 0; i < ALPHABET_SIZE; i++)
		nd.next[i] = 0;
	tree[++last_added] = nd;
	return last_added;
}

char active_edge() {
	return idx(text[active_e]);
}

void add_SL(int node) {
	if (needSL > 0) tree[needSL].slink = node;
	needSL = node;
}

bool walk_down(int node) {
	if (active_len >= tree[node].edge_length()) {
		active_e += tree[node].edge_length();
		active_len -= tree[node].edge_length();
		active_node = node;
		return true;
	}
	return false;
}

void st_init() {
	needSL = 0, last_added = 0, pos = -1, 
	remainder = 0, active_node = 0, active_e = 0, active_len = 0;
	root = active_node = new_node(-1, -1);
}

void st_extend(char c) {
	text[++pos] = c;
	needSL = 0;
	remainder++;
	while(remainder > 0) {
		if (active_len == 0) active_e = pos;
		if (tree[active_node].next[active_edge()] == 0) {
			int leaf = new_node(pos);
			tree[active_node].next[active_edge()] = leaf;
			add_SL(active_node); //rule 2
		} else {
			int nxt = tree[active_node].next[active_edge()];
			if (walk_down(nxt)) continue; //observation 2
			if (text[tree[nxt].start + active_len] == c) { //observation 1
				active_len++;
				add_SL(active_node); //observation 3
				break;
			}
			int split = new_node(tree[nxt].start, tree[nxt].start + active_len);
			tree[active_node].next[active_edge()] = split;
			int leaf = new_node(pos);
			tree[split].next[idx(c)] = leaf;
			tree[nxt].start += active_len;
			tree[split].next[idx(text[tree[nxt].start])] = nxt;
			add_SL(split); //rule 2
		}
		remainder--;
		if (active_node == root && active_len > 0) { //rule 1
			active_len--;
			active_e = pos - remainder + 1;
		} else
			active_node = tree[active_node].slink > 0 ? tree[active_node].slink : root; //rule 3
	}
}

int answer(const ResultList_t& result, unsigned long query) 
{
  auto it = std::lower_bound(result.cbegin(), result.cend(), query);
  if(it == result.cend()) {
    return result.back().length;
  } else {
    return it->length;
  }
}

TEST(Ukkonen2, test1)
{
  const std::string str("kfjskjfflsdfnsdfksjdfksdjfdsbfmdnsbfmsndbfmnsdbfmnsdbfmnsdbfmnsdbmfbsdkjhvkxjchvkxchiuvhkjdfnksjdhfkuhvjsdbfkjsdhfuyiuwyrjbfnsdkfjsdkjfbsdfsdmnfbsdmfjhsdgfzxbcmzfkjshfkjdshfkjdsfmspoasidkrwenmsdbfmsdbbfjhvxcbmvhsdjfhgsjdhgjgxgqhjhbndmfnsbdmfnbsdmfnbsdmfnbmsdnbfkjhvkxcjhvkjsdhkfsdhkjvxknsmbfmsdnfbmsdjxcvhdfnbvsdnfbvsdnfvjxchgvjdsfnsdbfvnsdbvfnsdhgvjhxcvsdbnfbvsdnfvbsndbvfnsbdsamdfnbdsmfbjhvjsdhfkjsdhkfjbdsnfsdnfvsdnbvfnsdbvfndsbvfndsbvfnsdjfhksdjhfksdjhfksjdhfsdvfnsdvnfvbsdbnfvdsniodasudkwqhmenbmfnbdsjhjhcvmbsnadsandasndbasbdmashdkjashdasdnbasvndbavsndasdhkasjhkjchzkjchkdmasnbdanmsddkjasndkjasnkhczuxasdasdasnbdmasnbdmasnbdmasnbdmasbdmasbdmsabdmnasbdmnasbdmansbdmasnbdmanskczhuasddasdagfsdiufydskjfdbfmsndbfmsndbkjhcvkjxchkvjsdbfmnsdbmfnbsdmnfbmdsfbmsdnbfmsdnbfmnsdbfmnsdbfmnsdbfmnsbdkjhxkcvhxdkasjhdsadnmasbdmnasbdmbasjkdhaksjdhkasjhdksahdkjashdkashdkjahskdhaskjdhkasjdhkasjhdkasjhdkasjhdkajshdkjashd");

  for(char c: str) {
    st_extend(c);
  }

  unsigned long counter = 0;
  // print(tree[root], "", counter);
  ResultList_t result;
  for(int i=0; i<ALPHABET_SIZE; ++i) {
    if(tree[root].next[i] > 0) {
      Mask_t mask;
      traverse(tree[tree[root].next[i]], counter, result, mask);
    }
  }
  std::cout << counter << std::endl;
  std::cout << result.size() << std::endl;
  for(auto r: result) {
    f << r.startIdx << " / " << static_cast<int>(r.length) << std::endl;
  }
  std::cout << answer(result, 6820) << std::endl;
}

} // namespace ukkonen2
} // namespace algo 