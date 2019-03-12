#pragma once

#include "constants.h"
#include "primitives.h"

struct Swap
{
    primitives::point_id_t a {constants::invalid_point};
    primitives::point_id_t b {constants::invalid_point};
    primitives::length_t improvement {0};
};

