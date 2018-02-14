#ifndef ALGO_GRAPHS_COMMON_HPP_
#define ALGO_GRAPHS_COMMON_HPP_

#include <gtest/gtest.h>

#include <algorithm>
#include <functional>
#include <limits>
#include <vector>

namespace algo {

using List = std::vector<int>;
using Flags = std::vector<bool>;

constexpr int MAX_INT = std::numeric_limits<int>::max();

using VerticeHandler = std::function<void(int)>;

} // namespace algo

#endif // ALGO_GRAPHS_COMMON_HPP_