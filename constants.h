#pragma once

#include "primitives.h"

namespace constants {

constexpr auto invalid_point{std::numeric_limits<primitives::point_id_t>::max()};
constexpr auto invalid_length{std::numeric_limits<primitives::length_t>::max()};

constexpr bool verbose {true};

} // namespace constants
