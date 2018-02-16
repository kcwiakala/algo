#include <Matrix.hpp>

#include "GraphsCommon.hpp"

namespace algo {
namespace hungarian {

struct BipartiteGraph
{
  BipartiteGraph(size_t l, size_t r): _M(l,r,-1) 
  {
  }

  void set(size_t i, const std::vector<int>& adj) 
  {
    for(int j=0; j<adj.size(); ++j) {
      _M(i,j) = adj[j];
    }
  }

  using Assignment = std::vector<int>;

  void assign(const Matrix<int>& M, Assignment& a)
  {
    const size_t H = M.height();
    const size_t W = M.width();

    a.assign(H,-1);
    std::vector<int> zerosPerRow(H, 0);
    std::vector<int> rows(H, 0);

    // Count how many zeros in row;
    for(int i=0; i<H; ++i) {
      rows[i] = i;
      const auto& row = M.row(i);
      zerosPerRow[i] = std::count(row.cbegin(), row.cend(), 0);
    }

    // Sort 
    auto comp = [&zerosPerRow](int r1, int r2) {
      return zerosPerRow[r1] < zerosPerRow[r2];
    };
    std::sort(rows.begin(), rows.end(), comp);

    Flags usedColumn(W, false);

    for(auto rit = rows.begin(); rit != rows.end(); ++rit) {
      if(zerosPerRow[*rit] > 0) {
        const auto& row = M.row(*rit);
        int j=0;
        for(; j<W; ++j) {
          if(!usedColumn[j] && row[j] == 0) {
            usedColumn[j] = true;
            for(int i=0; i<H; ++i) {
              if(M(i,j) == 0) {
                --zerosPerRow[i];
              }
            }
            break; 
          }
        }
        a[*rit] = j;
        // std::cout << "Assigning " << j << " for " << *rit << std::endl;
        std::sort(rit+1, rows.end(), comp);
      }
    }
  }

  void crossOut(Matrix<int>& M, const Assignment& a)
  {
    const size_t H = M.height();
    const size_t W = M.width();

    Flags markedRows(H, false);
    Flags markedCols(W, false);

    for(int i=0; i<H; ++i) {
      // Mark rows having no assignment
      if(a[i] == -1) {
        markedRows[i] = true;
        const auto& row = M.row(i);
        // Mark all (unmarked) columns having zeros in newly marked row
        for(int j=0; j<W; ++j) {
          if(row[j] == 0) {
            if(!markedCols[j]) {
              markedCols[j] = true;
              // Mark all rows having assignments in newly marked columns
              for(int k=0; k<H; ++k) {
                if(a[k] == j) {
                  markedRows[k] = true;
                }
              }
            }
          }
        }
      }
    }

    // Cross out marked columns and unmarked rows
    for(int i=0; i<H; ++i) {
      markedRows[i] = !markedRows[i];
    }

    // From remaining elements find the minimum
    int minVal = std::numeric_limits<int>::max();
    for(int i=0; i<H; ++i) {
      if(markedRows[i]) {
        continue;
      }
      for(int j=0; j<W; ++j) {
        if(!markedCols[j] && M(i,j) < minVal) {
          minVal = M(i,j);
        }
      }
    }

    // Subtract min from every unmarked element and add it to every element covered by two lines.
    for(int i=0; i<H; ++i) {
      for(int j=0; j<W; ++j) {
        if(markedRows[i] && markedCols[j]) {
          M(i,j) += minVal;
        } else if(!markedRows[i] && !markedCols[j]) {
          M(i,j) -= minVal;
        }
      }
    }
  }

  auto perfectAssignment() 
  {
    std::vector<int> assignment(_M.height(), -1);

    Matrix<int> H = _M;
    
    // Step 1
    for(int i=0; i<H.height(); ++i) {
      const auto& row = H.row(i);
      int minVal = *std::min_element(row.cbegin(), row.cend());
      for(int j=0; j<H.width(); ++j) {
        H(i,j) -= minVal;
      }
    }
    // std::cout << H << std::endl;

    // Step 2
    for(int j=0; j<H.width(); ++j) {
      const auto& col = H.column(j);
      int minVal = *std::min_element(col.cbegin(), col.cend());
      for(int i=0; i<H.height(); ++i) {
        H(i,j) -= minVal;
      }
    }
    // std::cout << H << std::endl;

    // Step 3
    int steps = H.width();
    while(steps--) {
      assign(H, assignment);
      if(std::find(assignment.cbegin(), assignment.cend(), -1) == assignment.cend()) {
        break;
      }
      crossOut(H, assignment);
      // std::cout << H << std::endl;
    }

    return assignment;
  }

  Matrix<int> _M;
};

TEST(Hungarian, test1) 
{
  // 3  5
  // 10 1
  BipartiteGraph g(2,2);
  g.set(0, {3,  5});
  g.set(1, {10, 1});

  auto a = g.perfectAssignment();

  for(int i: a) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
}

TEST(Hungarian, test2) 
{
  // 19 22 22 79 31 2 77 47 8 28 9 57 54 81 18 8 2 61 78 98 51 47 63 55 7 93 27 59 49 24 56 27 4 22 70 68 93 75 68 35 68 13 27 80 29 87 9 72 36 87 60 76 5 98 5 37 50 29 52 73 18 17 77 95 87 68 9 9 29 94 93 28 25 65 62 50 73 77 22 92 1 71 94 71 71 36 36 20 66 88 95 76 23 39 84 73 96 28 19 50 54 81 31 67 50 2 34 65 22 77 16 51 100 24 30 17 27 45 54 60 14 43 29 6 50 66 80 43 43 93 23 52 13 54 7 87 95 18 70 100 40 77 40 30 53 16 60 68 19 48 88 37 73 86 69 10 13 74 26 84 88 9 14 18 51 38 44 52 27 34 39 40 95 6 66 35 97 29 49 16 57 3 17 96 37 29 37 81 94 42 73 33 75 34 31 65 44 25 20 19 68 21 48 19 83 96 57 37 78 72 41 63 19 40 50 44 81 4 61 22 8 55 98 88 29 52 51 87 4 78 35 75 49 73 50 44 69 14 66 33 33 37 11 95 80 88 82 46 97 62 14 13 67 33 97 47 
  BipartiteGraph g(16, 16);
  g.set(0,  {19,22,22,79,31, 2,77,47, 8,28, 9,57,54,81,18, 8});
  g.set(1,  { 2,61,78,98,51,47,63,55, 7,93,27,59,49,24,56,27});
  g.set(2,  { 4,22,70,68,93,75,68,35,68,13,27,80,29,87, 9,72});
  g.set(3,  {36,87,60,76, 5,98, 5,37,50,29,52,73,18,17,77,95});
  g.set(4,  {87,68, 9, 9,29,94,93,28,25,65,62,50,73,77,22,92});
  g.set(5,  { 1,71,94,71,71,36,36,20,66,88,95,76,23,39,84,73});
  g.set(6,  {96,28,19,50,54,81,31,67,50, 2,34,65,22,77,16,51});
  g.set(7,  {100,24,30,17,27,45,54,60,14,43,29,6,50,66,80,43});
  g.set(8,  {43,93,23,52,13,54,7,87,95,18,70,100,40,77,40,30});
  g.set(9,  {53,16,60,68,19,48,88,37,73,86,69,10,13,74,26,84});
  g.set(10, {88, 9,14,18,51,38,44,52,27,34,39,40,95, 6,66,35});
  g.set(11, {97,29,49,16,57, 3,17,96,37,29,37,81,94,42,73,33});
  g.set(12, {75,34,31,65,44,25,20,19,68,21,48,19,83,96,57,37});
  g.set(13, {78,72,41,63,19,40,50,44,81, 4,61,22, 8,55,98,88});
  g.set(14, {29,52,51,87, 4,78,35,75,49,73,50,44,69,14,66,33});
  g.set(15, {33,37,11,95,80,88,82,46,97,62,14,13,67,33,97,47});


  auto a = g.perfectAssignment();

  for(int i: a) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
}

TEST(Hungarian, test3) 
{
  // 1 1 1
  // 9 1 8
  // 1 6 4
  BipartiteGraph g(3,3);
  g.set(0, {1, 1, 1});
  g.set(1, {9, 1, 8});
  g.set(2, {1, 6, 4});

  auto a = g.perfectAssignment();

  for(int i: a) {
    std::cout << i << " ";
  }
  std::cout << std::endl;


}

} // namspace hungarian
} // namespace algo