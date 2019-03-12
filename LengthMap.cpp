#include "LengthMap.h"

LengthMap::LengthMap(const std::vector<primitives::point_id_t>& ordered_points
    , const std::vector<primitives::space_t>& x
    , const std::vector<primitives::space_t>& y)
    : m_x(x)
    , m_y(y)
    , m_lengths(ordered_points.size())
{
    auto prev {ordered_points.back()};
    for (auto current : ordered_points)
    {
        insert(prev, current);
        prev = current;
    }
}

primitives::length_t LengthMap::length(primitives::point_id_t a, primitives::point_id_t b) const
{
    return m_lengths[std::min(a, b)].find(std::max(a, b))->second;
}

void LengthMap::insert(primitives::point_id_t a, primitives::point_id_t b)
{
    auto& map {m_lengths[std::min(a, b)]};
    map[std::max(a, b)] = compute_length(a, b);
}

