#include "oh/parser.h"

#include <array>
#include <iostream>

#include "cista/hash.h"

#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/abnf.hpp"

namespace oh {

/// https://www.bahn.de/service/individuelle-reise/barrierefrei
/// https://wiki.openstreetmap.org/wiki/Key:opening_hours/specification#section:rule_modifier
/// https://github.com/simonpoole/OpeningHoursParser/tree/master
/// https://products.aspose.app/pdf/de/conversion/pdf-to-csv
/// https://github.com/remi-dupre/opening-hours-rs/blob/master/opening-hours-syntax/src/grammar.pest

using namespace tao::pegtl;
using namespace abnf;

std::ostream& operator<<(std::ostream& out, rule const& r) {
  auto first = true;
  for (auto const& x : r.weekday_ranges_) {
    if (!first) {
      out << ", ";
    }
    first = false;
    out << x.from_ << "-" << x.to_;
  }
  out << " ";
  first = true;
  for (auto const& x : r.time_ranges_) {
    if (!first) {
      out << ", ";
    }
    first = false;
    out << x.from_ << "-" << x.to_;
  }
  return out;
}

constexpr cista::hash_t ihash(std::string_view s,
                              cista::hash_t h = cista::BASE_HASH) noexcept {
  auto const ptr = s.data();
  for (std::size_t i = 0U; i < s.size(); ++i) {
    h = cista::hash_combine(h, static_cast<std::uint8_t>(tolower(ptr[i])));
  }
  return h;
}

date::weekday to_weekday(std::string_view s) {
  switch (ihash(s)) {
    case cista::hash("su"): return date::Sunday;
    case cista::hash("mo"): return date::Monday;
    case cista::hash("tu"): return date::Tuesday;
    case cista::hash("we"): return date::Wednesday;
    case cista::hash("th"): return date::Thursday;
    case cista::hash("fr"): return date::Friday;
    case cista::hash("sa"): return date::Saturday;
    default:;
  }
  return date::Sunday;
}

hh_mm to_hh_mm(std::string_view s) {
  auto const to_int = [](char const c) { return c - '0'; };
  auto h = std::chrono::hours{10 * to_int(s[0]) + to_int(s[1])};
  auto m = std::chrono::minutes{10 * to_int(s[3]) + to_int(s[4])};
  return hh_mm{h + m};
}

struct su : istring<'s', 'u'> {};
struct mo : istring<'m', 'o'> {};
struct tu : istring<'t', 'u'> {};
struct we : istring<'w', 'e'> {};
struct th : istring<'t', 'h'> {};
struct fr : istring<'f', 'r'> {};
struct sa : istring<'s', 'a'> {};

struct wday : sor<su, mo, tu, we, th, fr, sa> {};
struct wday_range : sor<seq<wday, one<'-'>, wday>, wday> {};

struct time : seq<DIGIT, DIGIT, one<':'>, DIGIT, DIGIT> {};
struct time_range : seq<time, one<'-'>, time> {};

using time_list = list<time_range, string<',', ' '>>;
using wday_list = list<wday_range, string<',', ' '>>;
using rrule = seq<opt<seq<wday_list, one<' '>>>, opt<time_list>>;
using rule_list = list<rrule, string<';', ' '>>;

struct ruleset {
  std::array<hh_mm, 2> time_range_{};
  std::size_t time_idx_{0U};

  std::array<date::weekday, 2> weekday_range_{};
  std::size_t weekday_idx_{0U};

  rule rule_;

  ruleset_t rules_;
};

template <typename Rule>
struct action : tao::pegtl::nothing<Rule> {};

template <>
struct action<wday> {
  template <typename ActionInput>
  static void apply(const ActionInput& in, ruleset& x) {
    assert(x.weekday_idx_ <= 1);
    x.weekday_range_[x.weekday_idx_++] = to_weekday(in.string_view());
  }
};

template <>
struct action<wday_range> {
  template <typename ActionInput>
  static void apply(const ActionInput& in, ruleset& x) {
    x.rule_.weekday_ranges_.push_back(
        range<date::weekday>{x.weekday_range_[0], x.weekday_range_[1]});
    x.weekday_idx_ = 0U;
  }
};

template <>
struct action<time> {
  template <typename ActionInput>
  static void apply(const ActionInput& in, ruleset& x) {
    assert(x.weekday_idx_ <= 1);
    x.time_range_[x.time_idx_++] = to_hh_mm(in.string_view());
  }
};

template <>
struct action<time_range> {
  template <typename ActionInput>
  static void apply(const ActionInput& in, ruleset& x) {
    x.rule_.time_ranges_.push_back(
        range<hh_mm>{x.time_range_[0], x.time_range_[1]});
    x.time_idx_ = 0U;
  }
};

template <>
struct action<rrule> {
  template <typename ActionInput>
  static void apply(const ActionInput& in, ruleset& x) {
    x.rules_.emplace_back(std::move(x.rule_));
    x.rule_.time_ranges_.clear();
    x.rule_.weekday_ranges_.clear();
  }
};

ruleset_t parse(std::string_view s) {
  using grammar = must<rule_list>;
  auto input = memory_input{s, s};
  auto r = ruleset{};
  parse<grammar, action>(input, r);
  return r.rules_;
}

}  // namespace oh