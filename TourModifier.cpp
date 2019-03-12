#include "TourModifier.h"

TourModifier::TourModifier(const TourModifier& tour)
    : TourModifier(tour.order(), tour.m_length_map.x(), tour.m_length_map.y()) {}

TourModifier::TourModifier(const std::vector<primitives::point_id_t>& initial_tour
     , const std::vector<primitives::space_t>& x
     , const std::vector<primitives::space_t>& y)
    : m_length_map(initial_tour, x, y)
    , m_adjacents(initial_tour.size(), {constants::invalid_point, constants::invalid_point})
    , m_next(initial_tour.size(), constants::invalid_point)
{
    reset_adjacencies(initial_tour);
    update_next();
}

void TourModifier::reset_adjacencies(const std::vector<primitives::point_id_t>& initial_tour)
{
    auto prev = initial_tour.back();
    for (auto p : initial_tour)
    {
        create_adjacency(p, prev);
        prev = p;
    }
}

primitives::point_id_t TourModifier::prev(primitives::point_id_t i) const
{
    const auto next {m_next[i]};
    if (m_adjacents[i][0] == next)
    {
        return m_adjacents[i][1];
    }
    else if (m_adjacents[i][1] == next)
    {
        return m_adjacents[i][0];
    }
    else
    {
        std::cout << __func__ << ": error: could not determine previous point." << std::endl;
        std::abort();
    }
}

primitives::length_t TourModifier::length() const
{
    primitives::length_t sum {0};
    for (primitives::point_id_t i {0}; i < m_next.size(); ++i)
    {
        sum += m_length_map.length(i, m_next[i]);
    }
    return sum;
}

primitives::length_t TourModifier::length(primitives::point_id_t i) const
{
    return m_length_map.length(i, m_next[i]);
}

std::vector<primitives::point_id_t> TourModifier::order() const
{
    constexpr primitives::point_id_t start {0};
    primitives::point_id_t current {start};
    std::vector<primitives::point_id_t> ordered_points;
    primitives::point_id_t count {0};
    do
    {
        ordered_points.push_back(current);
        current = m_next[current];
        if (count > m_next.size())
        {
            std::cout << __func__ << ": error: too many traversals." << std::endl;
            std::abort();
        }
        ++count;
    } while (current != start);
    return ordered_points;
}

void TourModifier::move(primitives::point_id_t a, primitives::point_id_t b)
{
    m_length_map.erase(a, m_next[a]);
    m_length_map.erase(b, m_next[b]);
    m_length_map.insert(a, b);
    m_length_map.insert(m_next[a], m_next[b]);
    break_adjacency(a);
    break_adjacency(b);
    create_adjacency(a, b);
    create_adjacency(m_next[a], m_next[b]);
    update_next();
}

void TourModifier::update_next()
{
    primitives::point_id_t current{0};
    m_next[current] = m_adjacents[current].front();
    do
    {
        auto prev = current;
        current = m_next[current];
        m_next[current] = get_other(current, prev);
    } while (current != 0); // tour cycle condition.
}

primitives::point_id_t TourModifier::get_other(primitives::point_id_t point, primitives::point_id_t adjacent) const
{
    const auto& a = m_adjacents[point];
    if (a.front() == adjacent)
    {
        return a.back();
    }
    else
    {
        return a.front();
    }
}

void TourModifier::create_adjacency(primitives::point_id_t point1, primitives::point_id_t point2)
{
    fill_adjacent(point1, point2);
    fill_adjacent(point2, point1);
}

void TourModifier::fill_adjacent(primitives::point_id_t point, primitives::point_id_t new_adjacent)
{
    if (m_adjacents[point].front() == constants::invalid_point)
    {
        m_adjacents[point].front() = new_adjacent;
    }
    else if (m_adjacents[point].back() == constants::invalid_point)
    {
        m_adjacents[point].back() = new_adjacent;
    }
    else
    {
        std::cout << "No availble slot for new adjacent." << std::endl;
        std::abort();
    }
}

void TourModifier::break_adjacency(primitives::point_id_t i)
{
    break_adjacency(i, m_next[i]);
}

void TourModifier::break_adjacency(primitives::point_id_t point1, primitives::point_id_t point2)
{
    vacate_adjacent_slot(point1, point2, 0);
    vacate_adjacent_slot(point1, point2, 1);
    vacate_adjacent_slot(point2, point1, 0);
    vacate_adjacent_slot(point2, point1, 1);
}

void TourModifier::vacate_adjacent_slot(primitives::point_id_t point, primitives::point_id_t adjacent, int slot)
{
    if (m_adjacents[point][slot] == adjacent)
    {
        m_adjacents[point][slot] = constants::invalid_point;
    }
}

