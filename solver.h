#pragma once

#include "Swap.h"
#include "TourModifier.h"
#include "constants.h"
#include "primitives.h"

#include <vector>

namespace solver {

inline primitives::length_t compute_improvement(const TourModifier& tour
    , primitives::point_id_t i
    , primitives::point_id_t j
    , primitives::length_t current_length)
{
    auto new_length {tour.length_map().compute_length(i, j)};
    if (new_length > current_length)
    {
        return 0;
    }
    new_length += tour.length_map().compute_length(tour.next(i), tour.next(j));
    if (new_length > current_length)
    {
        return 0;
    }
    if (new_length < current_length)
    {
        return current_length - new_length;
    }
    return 0;
}

inline Swap first_improvement(const TourModifier& tour)
{
    constexpr primitives::point_id_t start {0};
    // first segment cannot be compared with last segment.
    auto end {tour.prev(start)};
    const auto first_old_length {tour.length(start)};
    for (primitives::point_id_t i {tour.next(tour.next(start))}; i != end; i = tour.next(i))
    {
        const auto current_length {first_old_length + tour.length(i)};
        const auto improvement {compute_improvement(tour, start, i, current_length)};
        if (improvement > 0)
        {
            return {start, i, improvement};
        }
    }

    end = tour.prev(end);
    for (primitives::point_id_t i {tour.next(start)}; i != end; i = tour.next(i))
    {
        const auto first_old_length {tour.length(i)};
        auto j {tour.next(tour.next(i))};
        while (j != start)
        {
            const auto current_length {first_old_length + tour.length(j)};
            const auto improvement {compute_improvement(tour, i, j, current_length)};
            if (improvement > 0)
            {
                return {i, j, improvement};
            }
            j = tour.next(j);
        }
    }
    return {};
}

inline void hill_climb(TourModifier& tour_modifier)
{
    auto move {first_improvement(tour_modifier)};
    int iteration{1};
    while (move.improvement > 0)
    {
        tour_modifier.move(move.a, move.b);
        if (constants::verbose)
        {
            auto length {tour_modifier.length()};
            std::cout << "Iteration " << iteration
                << " tour length: " << length
                << " (step improvement: " << move.improvement << ")\n";
        }
        move = first_improvement(tour_modifier);
        ++iteration;
    }
}

} // namespace solver
