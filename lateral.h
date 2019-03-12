#pragma once

#include "Pair.h"
#include "Swap.h"
#include "TourModifier.h"
#include "solver.h"

#include <algorithm> // min

namespace lateral {

inline bool is_valid_move(const TourModifier& tour
    , primitives::point_id_t i
    , primitives::point_id_t j
    , primitives::length_t current_length
    , primitives::length_t desired_cost
    , primitives::length_t& next_cost)
{
    auto new_length {tour.length_map().compute_length(i, j)};
    new_length += tour.length_map().compute_length(tour.next(i), tour.next(j));
    const auto target_length {current_length + desired_cost};
    if (new_length > target_length)
    {
        next_cost = std::min(next_cost, desired_cost + new_length - target_length);
    }
    return new_length == target_length;
}

inline std::vector<Swap> find_swaps(const TourModifier& tour
    , primitives::length_t cost
    , primitives::length_t& next_cost)
{
    std::vector<Swap> swaps;
    constexpr primitives::point_id_t start {0};
    // first segment cannot be compared with last segment.
    auto end {tour.prev(start)};
    const auto first_old_length {tour.length(start)};
    for (primitives::point_id_t i {tour.next(tour.next(start))}; i != end; i = tour.next(i))
    {
        const auto current_length {first_old_length + tour.length(i)};
        if (is_valid_move(tour, start, i, current_length, cost, next_cost))
        {
            swaps.push_back({start, i, cost});
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
            if (is_valid_move(tour, i, j, current_length, cost, next_cost))
            {
                swaps.push_back({i, j, cost});
            }
            j = tour.next(j);
        }
    }
    return swaps;
}

inline Swap restricted_first_improvement(const TourModifier& tour, const Pair& restriction)
{
    constexpr primitives::point_id_t start {0};
    // first segment cannot be compared with last segment.
    auto end {tour.prev(start)};
    const auto first_old_length {tour.length(start)};
    const Segment first_segment(start, tour.next(start));
    for (primitives::point_id_t i {tour.next(tour.next(start))}; i != end; i = tour.next(i))
    {
        const auto current_length {first_old_length + tour.length(i)};
        const auto improvement {solver::compute_improvement(tour, start, i, current_length)};
        if (improvement > 0)
        {
            const Pair removed(first_segment, Segment(i, tour.next(i)));
            if (restriction != removed)
            {
                return {start, i, improvement};
            }
        }
    }

    end = tour.prev(end);
    for (primitives::point_id_t i {tour.next(start)}; i != end; i = tour.next(i))
    {
        const auto first_old_length {tour.length(i)};
        auto j {tour.next(tour.next(i))};
        const Segment first_segment(i, tour.next(i));
        while (j != start)
        {
            const auto current_length {first_old_length + tour.length(j)};
            const auto improvement {solver::compute_improvement(tour, i, j, current_length)};
            if (improvement > 0)
            {
                const Pair removed(first_segment, Segment(j, tour.next(j)));
                if (restriction != removed)
                {
                    return {i, j, improvement};
                }
            }
            j = tour.next(j);
        }
    }
    return {};
}

inline TourModifier perturbation_climb(const std::vector<Swap>& swaps, const TourModifier& tour)
{
    const auto original_length {tour.length()};
    const auto original_points {tour.order()};
    for (const auto& swap : swaps)
    {
        TourModifier new_tour(tour);
        new_tour.move(swap.a, swap.b);
        const Pair restriction(Segment(swap.a, swap.b), Segment(tour.next(swap.a), tour.next(swap.b)));
        while (true)
        {
            const auto new_swap {restricted_first_improvement(new_tour, restriction)};
            if (new_swap.improvement == 0)
            {
                break;
            }
            new_tour.move(new_swap.a, new_swap.b);
        }
        solver::hill_climb(new_tour);
        if (new_tour.length() < original_length)
        {
            return new_tour;
        }
    }
    return tour;
}

inline TourModifier perturbation_climb(const TourModifier& tour, primitives::length_t cost, primitives::length_t& next_cost)
{
    const auto swaps {find_swaps(tour, cost, next_cost)};
    return perturbation_climb(swaps, tour);
}

inline TourModifier perturbation_climb(const TourModifier& tour)
{
    const auto original_length {tour.length()};
    primitives::length_t current_cost {0};
    while (true)
    {
        std::cout << "trying perturbation cost: " << current_cost << std::endl;
        primitives::length_t next_cost {constants::invalid_length};
        const auto new_tour {perturbation_climb(tour, current_cost, next_cost)};
        if (new_tour.length() < original_length)
        {
            return new_tour;
        }
        if (next_cost == constants::invalid_length)
        {
            break;
        }
        current_cost = next_cost;
    }
    std::cout << "No more perturbations left to try." << std::endl;
    return tour;
}

} // namespace lateral
