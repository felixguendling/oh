#pragma once

#include <vector>

#include "date/date.h"

namespace oh {

template <typename T>
struct range {
  T from_, to_;
};

using hh_mm = date::hh_mm_ss<std::chrono::minutes>;

struct rule {
  friend std::ostream& operator<<(std::ostream&, rule const&);

  std::vector<range<date::weekday>> weekday_ranges_;
  std::vector<range<hh_mm>> time_ranges_;
};

using ruleset_t = std::vector<rule>;

ruleset_t parse(std::string_view);

}  // namespace oh