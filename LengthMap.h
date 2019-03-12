#pragma once

#include "primitives.h"

#include <algorithm> // min, max
#include <cmath> // sqrt
#include <unordered_map>
#include <vector>

class LengthMap
{
public:
    LengthMap(const std::vector<primitives::point_id_t>& ordered_points
        , const std::vector<primitives::space_t>& x
        , const std::vector<primitives::space_t>& y);

    primitives::length_t length(primitives::point_id_t a, primitives::point_id_t b) const;

    void insert(primitives::point_id_t a, primitives::point_id_t b);

    primitives::length_t compute_length(primitives::point_id_t a, primitives::point_id_t b) const
    {
        auto dx = m_x[a] - m_x[b];
        auto dy = m_y[a] - m_y[b];
        auto exact = std::sqrt(dx * dx + dy * dy);
        return exact + 0.5; // return type cast.
    }

    void erase(primitives::point_id_t a, primitives::point_id_t b)
    {
        m_lengths[std::min(a, b)].erase(std::max(a, b));
    }

    const std::vector<primitives::space_t>& x() const { return m_x; }
    const std::vector<primitives::space_t>& y() const { return m_y; }

    const primitives::space_t& x(primitives::point_id_t i) const { return m_x[i]; }
    const primitives::space_t& y(primitives::point_id_t i) const { return m_y[i]; }

private:
    std::vector<primitives::space_t> m_x;
    std::vector<primitives::space_t> m_y;

    std::vector<std::unordered_map<primitives::point_id_t, primitives::length_t>> m_lengths;

};
