#pragma once

#include <iostream>
#include <vector>

namespace algo {

template<typename T>
class Matrix
{
public:
  using Array=std::vector<T>;

  Matrix(size_t w, size_t h, const T& init): 
    _h(h), 
    _w(w), 
    _col(h, init), 
    _m(h, Array(w, init))
  {}

  Matrix(const Matrix& rhs) = default;

  T& operator()(size_t i, size_t j)
  {
    return _m.at(i).at(j);
  }

  const T& operator()(size_t i, size_t j) const
  {
    return _m.at(i).at(j);
  }

  const Array& row(size_t i) const
  {
    return _m.at(i);
  }

  const Array& column(size_t j) const
  {
    for(int i=0; i<_h; ++i) {
      _col[i] = _m[i].at(j);
    }
    return _col;
  }

  size_t height() const
  {
    return _h;
  }

  size_t width() const
  {
    return _w;
  }

private:
  const size_t _h;
  const size_t _w;
  mutable Array _col;
  std::vector<Array> _m;
};

} // namespace algo

template<typename T>
std::ostream& operator<<(std::ostream& strm, const algo::Matrix<T>& m)
{
  for(int i=0; i<m.height(); ++i) {
    for(int j=0; j<m.width(); ++j) {
      strm << m(i,j) << " ";
    }
    strm << std::endl;
  }
  return strm;
}