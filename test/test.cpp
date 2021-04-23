#include <vector>

#include <gtest/gtest.h>

#include "itertools/enumerate.hpp"

namespace test {

TEST(TestEnumerate, TestSimple) {
  std::size_t expected_i = 0;
  std::vector arr{1, 2, 3};

  for (auto [i, elem] : itertools::enumerate(arr)) {
    ASSERT_EQ(i, expected_i);
    ASSERT_EQ(elem, arr[i]);
    ++expected_i;
  }

  ASSERT_EQ(expected_i, arr.size());
}

/*

TEST(TestEnumerate, TestChain) {
  std::size_t expected_i = 0;
  std::vector arr{1, 2, 3};

  for (auto [i, elem] : arr | itertools::enumerate()) {
    ASSERT_EQ(i, expected_i);
    ASSERT_EQ(elem, arr[i]);
    ++expected_i;
  }

  ASSERT_EQ(expected_i, arr.size());
}
*/

}  // namespace test
