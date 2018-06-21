#include <Common.hpp>
#include <bitset>

namespace algo {

class Sudoku
{
private: 
  using flags=std::bitset<9>;
public:
  Sudoku() {
    for(int i=0; i<9; ++i) {
      for(int j=0; j<9; ++j) {
        a[i][j] = 0;
      }
    }
  }

  void set(int i, int j, int value) {
    a[i][j] = value;
  }

  size_t remaining() const {
    size_t result = 0;
    for(int i=0; i<9; ++i) {
      for(int j=0; j<9; ++j) {
        result += (a[i][j] == 0);
      }
    }
    return result;
  }

  void print() const {
    for(int i=0; i<9; ++i) {
      for(int j=0; j<9; ++j) {
        std::cout << a[i][j] << " ";
      }
      std::cout << std::endl;
    }
  }

  std::pair<int, int> mostConstrained(flags& available) const {
    std::pair<int, int> result(-1, -1);
    flags rows[9];
    flags columns[9];
    flags sectors[3][3];
    for(int i=0; i<9; ++i) {
      for(int j=0; j<9; ++j) {
        if(a[i][j] > 0) {
          columns[i].set(a[i][j]-1);
          rows[j].set(a[i][j]-1);
          sectors[i/3][j/3].set(a[i][j]-1);
        }
      }
    }
    for(int i=0; i<9; ++i) {
      for(int j=0; j<9; ++j) {
        if(a[i][j] == 0) {
          flags aux = columns[i] | rows[j] | sectors[i/3][j/3];
          aux.flip();
          // std::cout << aux.count() << " ";
          if(aux.count() < available.count()) {
            result.first = i;
            result.second = j;
            available = aux;
          }
        } else {
          // std::cout << "X ";
        }
      }
      // std::cout << std::endl;
    }
    // std::cout << available.count() << std::endl;
    return result;
  }

  struct Step 
  {
    Step(int iX, int iY, int iV): x(iX), y(iY), v(iV) {}
    int x;
    int y;
    int v;
  };

  void solve()
  {
    size_t rem = remaining();
    std::vector<Step> steps;
    steps.reserve(rem);
    bool solved = false;
    solve(steps, rem, solved);
  }

private:
  void solve(std::vector<Step>& steps, int maxSteps, bool& solved) {
    if(steps.size() == maxSteps) {
      print();
      solved = true;
    } else {
      flags available;
      available.flip();
      auto coord = mostConstrained(available);
      // std::cout << coord.first << " " << coord.second << " " << available << std::endl;
      for(int i=0; i<9; ++i) {
        if(available.test(i)) {
          steps.emplace_back(coord.first, coord.second, i+1);
          a[coord.first][coord.second] = i+1; 
          solve(steps, maxSteps, solved);
          steps.pop_back();
          a[coord.first][coord.second] = 0; 
          if(solved) {
            return;
          }
        }
      }
    }
  }

private:
  int a[9][9];
};

TEST(sudoku, hard) {
  Sudoku s;
  s.set(0,7,1);
  s.set(0,8,2);
  s.set(1,4,3);
  s.set(1,5,5);
  s.set(2,3,6);
  s.set(2,7,7);
  s.set(3,0,7);
  s.set(3,6,3);
  s.set(4,3,4);
  s.set(4,6,8);
  s.set(5,0,1);
  s.set(6,3,1);
  s.set(6,4,2);
  s.set(7,1,8);
  s.set(7,7,4);
  s.set(8,1,5);
  s.set(8,6,6);

  s.print();
  std::cout << std::endl;
  
  s.solve();
}

} // namespace algo