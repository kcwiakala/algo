#include <gtest/gtest.h>
#include <iostream>

#include <algorithm>
#include <cmath>
#include <vector>
#include <stack>

using Rule = std::pair<char, char>; // Denoting first smaller than second

Rule getRule(const std::string& s1, const std::string& s2)
{
  int size = std::min(s1.size(), s2.size());
  for(int i=0; i<size; ++i) {
    if(s1[i] != s2[i]) {
      return Rule(s1[i], s2[i]);
    }
  }
  return Rule(0,0);
}

struct Node 
{
  Node(int s, char c): value(c), next(s, false) {}

  bool isRoot = true;
  char value;
  std::vector<bool> next;
};

struct Graph
{
  Graph(int s): size(s)
  {
    nodes.reserve(size);
    for(int i=0; i<size; ++i) {
      nodes.emplace_back(Node(size, 'a' + i));
    }
  }

  int nidx(char c) const
  {
    return c-'a';
  }

  void addRule(const Rule& r)
  {
    nodes[nidx(r.first)].next[nidx(r.second)] = true;
    nodes[nidx(r.second)].isRoot = false;
  }

  std::string order()
  {
    std::string result;;
    std::vector<int> topo(size, -1);
    std::vector<bool> visited(size, false);
    std::stack<int> stack;

    for(auto& node: nodes) {
      result.push_back(node.value);
      if(node.isRoot) {
        // std::cout << "Node " << node.value << " is a root" <<  std::endl;
        int level = 0;
        stack.push(nidx(node.value));
        while(!stack.empty()) {
          int idx = stack.top();
          stack.pop();
          ++level;

          Node& n = nodes[idx];
          if(topo[idx] < level) {
            topo[idx] = level;
          } else {
            // std::cout << "Skipping node " << n.value << " on level " << level << std::endl;
            continue;
          }

          // std::cout << "Visiting node " << n.value << " on level " << level << std::endl;
          
          bool hasNext = false;
          for(int i=0; i<size; ++i) {
            if(n.next[i]) {
              stack.push(i);
              hasNext = true;
            }
          }
          if(!hasNext) {
            // std::cout << "Node " << n.value << " is a leaf" <<  std::endl;
            --level;
          } 
        }
      }
    }
    // std::cout << "Result before sorting " << result << std::endl;
    std::sort(result.begin(), result.end(), [this, &topo](char c1, char c2) {
      return topo[nidx(c1)] < topo[nidx(c2)];
    });
    // std::cout << "Result after sorting " << result << std::endl;
    return result;
  }

  const int size;
  std::vector<Node> nodes;
};

std::string printOrder(std::string dict[], int N, int k)
{
  Graph g(k);
  for(int i=1; i<N; ++i) {
    Rule r = getRule(dict[i-1], dict[i]);
    if(r.first != 0) {
      // std::cout << "Found rule " << r.first << " < " << r.second << std::endl;
      g.addRule(r);
    }
  }

  return g.order();
}

TEST(AlienDictionary, test1) 
{
  // Input:  Dict[] = { "baa", "abcd", "abca", "cab", "cad" }, k = 4
  // Output: Function returns "bdac" 

  std::string dict[] = { "baa", "abcd", "abca", "cab", "cad" };
  EXPECT_EQ(printOrder(dict, 5, 4), "bdac");

  // Input:  Dict[] = { "caa", "aaa", "aab" }, k = 3
  // Output: Function returns "cab"
  std::string dict2[] = { "caa", "aaa", "aab" };
  EXPECT_EQ(printOrder(dict2, 3, 3), "cab");
}

// 20 12
// aagkllkackabaicaaackhehjcfjaieh aifheecdiaifgigad baciifif bdddbkacejgckajjfced beh bgajaheailelicfgbcffifdilgaealjk bgccbbcbjibl ccghfleijliglaidjjdkhaklhad ckbjfaeidhlbkiagi ckgifb daclag fcbkeeebclifafhbhiabdkbcejcbdifdhjhkeiaicgllhacdka fhfgikadkkgegbigkhahhdakhgckdahij fkbabfikfckkkdaeegehjhgelbkalaebfjlcdflbafkgeicbl hcggjbdjdlhekdbbibdfhhdggdgjkakgegjaeflaea hgkeacjgeclbjgjcg hhcgicdggbjhdbfgchhlcckfcdjdacckb hlkjjliihfilhc icheefbhcbhdajkacflfcfcgcfjijekkaffjh jcgigcbbbfklkajfdfajjiigace
TEST(AlienDictionary, test2) 
{
  std::string dict[] = {"aagkllkackabaicaaackhehjcfjaieh", "aifheecdiaifgigad", "baciifif", 
    "bdddbkacejgckajjfced", "beh", "bgajaheailelicfgbcffifdilgaealjk", "bgccbbcbjibl", 
    "ccghfleijliglaidjjdkhaklhad", "ckbjfaeidhlbkiagi", "ckgifb", "daclag", 
    "fcbkeeebclifafhbhiabdkbcejcbdifdhjhkeiaicgllhacdka", "fhfgikadkkgegbigkhahhdakhgckdahij",
    "fkbabfikfckkkdaeegehjhgelbkalaebfjlcdflbafkgeicbl", "hcggjbdjdlhekdbbibdfhhdggdgjkakgegjaeflaea",
    "hgkeacjgeclbjgjcg", "hhcgicdggbjhdbfgchhlcckfcdjdacckb", "hlkjjliihfilhc", 
    "icheefbhcbhdajkacflfcfcgcfjijekkaffjh", "jcgigcbbbfklkajfdfajjiigace"};

  
  printOrder(dict, 20, 12);
}