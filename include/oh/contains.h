#pragma once

#include "date/date.h"

#include "oh/parser.h"

namespace oh {

using local_minutes = date::local_time<std::chrono::minutes>;

bool contains(ruleset_t const&, local_minutes);

}  // namespace oh