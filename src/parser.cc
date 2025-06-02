#include "oh/parser.h"

#include <array>
#include <iostream>

#include "cista/hash.h"

#include "tao/pegtl.hpp"
#include "tao/pegtl/contrib/abnf.hpp"
#include "utl/parser/arg_parser.h"

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

date::month to_month(std::string_view s) {
  switch (ihash(s)) {
    case cista::hash("jan"): return date::January;
    case cista::hash("feb"): return date::February;
    case cista::hash("mar"): return date::March;
    case cista::hash("apr"): return date::April;
    case cista::hash("may"): return date::May;
    case cista::hash("jun"): return date::June;
    case cista::hash("jul"): return date::July;
    case cista::hash("aug"): return date::August;
    case cista::hash("sep"): return date::September;
    case cista::hash("oct"): return date::October;
    case cista::hash("nov"): return date::November;
    case cista::hash("dec"): return date::December;
    default:;
  }
  return date::January;
}

date::day to_day(std::string_view s) {
  return date::day{utl::parse<unsigned>(s)};
}

hh_mm to_hh_mm(std::string_view s) {
  auto const to_int = [](char const c) { return c - '0'; };
  auto h = std::chrono::hours{10 * to_int(s[0]) + to_int(s[1])};
  auto m = std::chrono::minutes{10 * to_int(s[3]) + to_int(s[4])};
  return hh_mm{h + m};
}

struct d01 : istring<'0', '1'> {};
struct d02 : istring<'0', '2'> {};
struct d03 : istring<'0', '3'> {};
struct d04 : istring<'0', '4'> {};
struct d05 : istring<'0', '5'> {};
struct d06 : istring<'0', '6'> {};
struct d07 : istring<'0', '7'> {};
struct d08 : istring<'0', '8'> {};
struct d09 : istring<'0', '9'> {};
struct d10 : istring<'1', '0'> {};
struct d11 : istring<'1', '1'> {};
struct d12 : istring<'1', '2'> {};
struct d13 : istring<'1', '3'> {};
struct d14 : istring<'1', '4'> {};
struct d15 : istring<'1', '5'> {};
struct d16 : istring<'1', '6'> {};
struct d17 : istring<'1', '7'> {};
struct d18 : istring<'1', '8'> {};
struct d19 : istring<'1', '9'> {};
struct d20 : istring<'2', '0'> {};
struct d21 : istring<'2', '1'> {};
struct d22 : istring<'2', '2'> {};
struct d23 : istring<'2', '3'> {};
struct d24 : istring<'2', '4'> {};
struct d25 : istring<'2', '5'> {};
struct d26 : istring<'2', '6'> {};
struct d27 : istring<'2', '7'> {};
struct d28 : istring<'2', '8'> {};
struct d29 : istring<'2', '9'> {};
struct d30 : istring<'3', '0'> {};
struct d31 : istring<'3', '1'> {};

struct daynum : sor<d01,
                    d02,
                    d03,
                    d04,
                    d05,
                    d06,
                    d07,
                    d08,
                    d09,
                    d10,
                    d11,
                    d12,
                    d13,
                    d14,
                    d15,
                    d16,
                    d17,
                    d18,
                    d19,
                    d20,
                    d21,
                    d22,
                    d23,
                    d24,
                    d25,
                    d26,
                    d27,
                    d28,
                    d29,
                    d30,
                    d31> {};

struct jan : istring<'j', 'a', 'n'> {};
struct feb : istring<'f', 'e', 'b'> {};
struct mar : istring<'m', 'a', 'r'> {};
struct apr : istring<'a', 'p', 'r'> {};
struct may : istring<'m', 'a', 'y'> {};
struct jun : istring<'j', 'u', 'n'> {};
struct jul : istring<'j', 'u', 'l'> {};
struct aug : istring<'a', 'u', 'g'> {};
struct sep : istring<'s', 'e', 'p'> {};
struct oct : istring<'o', 'c', 't'> {};
struct nov : istring<'n', 'o', 'v'> {};
struct dec : istring<'d', 'e', 'c'> {};

struct month : sor<jan, feb, mar, apr, may, jun, jul, aug, sep, oct, nov, dec> {
};

struct year : seq<DIGIT, DIGIT, DIGIT, DIGIT> {};

struct date_from : sor<seq<month, one<' '>, daynum>, month> {};
struct monthday_range : sor<date_from, seq<date_from, one<'-'>, date_from>> {};
struct monthday_list : list<monthday_range, string<',', ' '>>;

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
using full_rule = seq<seq<wday_list, one<' '>, time_list>>;
using rrule = sor<full_rule, wday_list, time_list>;
using rule_list = list<rrule, string<';', ' '>>;

struct ruleset {
  std::array<hh_mm, 2> time_range_{};
  std::size_t time_idx_{0U};

  std::array<date::weekday, 2> weekday_range_{};
  std::size_t weekday_idx_{0U};

  std::array<monthday, 2> monthday_range_{};
  std::size_t monthday_idx_{0U};

  rule rule_;

  ruleset_t rules_;
};

template <typename Rule>
struct action : tao::pegtl::nothing<Rule> {};

template <>
struct action<monthday> {
  template <typename ActionInput>
  static void apply(const ActionInput& in, ruleset& x) {
    assert(x.weekday_idx_ <= 1);
    x.monthday_range_[x.monthday_idx_++] = to_monthday(in.string_view());
  }
};

template <>
struct action<monthday_range> {
  template <typename ActionInput>
  static void apply(const ActionInput& in, ruleset& x) {
    x.rule_.monthday_ranges_.push_back(
        range<monthday>{x.monthday_range_[0], x.monthday_range_[1]});
    x.monthday_idx_ = 0U;
  }
};

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