#include "oh/contains.h"

#include <iostream>

#include "utl/helpers/algorithm.h"

namespace oh {

bool contains(rule const& r, local_minutes const& t) {
  auto const iso =
      date::weekday{std::chrono::time_point_cast<date::days>(t)}.iso_encoding();
  auto const contains_weekday =
      utl::any_of(r.weekday_ranges_, [&](range<date::weekday> const& wdr) {
        return wdr.from_.iso_encoding() <= iso && iso <= wdr.to_.iso_encoding();
      });
  if (!contains_weekday) {
    return false;
  }
  auto const hm =
      hh_mm{t.time_since_epoch() % std::chrono::minutes{1440}}.to_duration();
  auto const contains_time =
      utl::any_of(r.time_ranges_, [&](range<hh_mm> const& tr) {
        return tr.from_.to_duration() <= hm && hm <= tr.to_.to_duration();
      });
  return contains_time;
}

bool contains(ruleset_t const& r,
              date::local_time<std::chrono::minutes> const t) {
  return utl::any_of(r, [&](rule const& x) { return contains(x, t); });
}

}  // namespace oh