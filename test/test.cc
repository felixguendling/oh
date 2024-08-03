#include "gtest/gtest.h"

#include "oh/contains.h"
#include "oh/parser.h"

using namespace oh;
using namespace date;
using namespace std::chrono_literals;

TEST(a, b) {
  auto r = parse(
      "Mo-Fr 07:50-12:00, 12:45-18:10; "
      "Sa 08:50-13:40, 14:25-19:10; "
      "Su 08:50-13:45, 14:45-21:10");
  EXPECT_FALSE(
      contains(r, local_minutes{local_days{2024_y / August / 3} + 8h + 49min}));
  EXPECT_TRUE(
      contains(r, local_minutes{local_days{2024_y / August / 3} + 8h + 50min}));
}