/*******************************************************************************
 * icfp/2012/source/delta_t.hpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_DELTA_T_HPP
#define ICFP_2012_SOURCE_DELTA_T_HPP

#include <cassert>
#include <cstddef>

#include <algorithm>
#include <deque>
#include <map>
#include <utility>

#include <boost/functional/hash.hpp>

#include "index_t.hpp"
#include "move_is_valid.hpp"
#include "state_t.hpp"

namespace icfp2012
{

struct delta_t
{
    state_t const & base;

    std::map< index_t, char > cell_map;

    index_t robot_index;
    std::deque< index_t > active_indices;

    unsigned int n_turns;
    unsigned int n_lambdas_remaining;
    bool robot_is_destroyed;
    unsigned int n_turns_underwater;
    unsigned int n_razors;

    explicit delta_t(state_t const & base_);
    delta_t(state_t const & base_, int);

    delta_t& operator=(delta_t other);
    void swap(delta_t& other);

    int score() const;
    unsigned int water_level() const;

private:
    class bracket_proxy;
public:
    bracket_proxy operator[](index_t const & index);
    char operator[](index_t const index) const;

    bool move_is_valid(char const move) const;

    delta_t move_robot_update(char const move) const;

    bool partial_equal(delta_t const & other) const;
    bool partial_less(delta_t const & other) const;

    bool operator==(delta_t const & other) const;
    bool operator!=(delta_t const & other) const;

    std::size_t hash_value() const;
    inline friend std::size_t
    hash_value(delta_t const & this_)
    { return this_.hash_value(); }
};

/*******************************************************************************
 ******************************************************************************/

class delta_t::bracket_proxy
{
    delta_t& this_;
    index_t const index;
    explicit bracket_proxy(delta_t& _this_, index_t const & index_)
        : this_(_this_), index(index_)
    { }
    friend struct delta_t;
public:

    operator char() const
    { return const_cast< delta_t const & >(this_)[index]; }

    bracket_proxy const &
    operator=(char const cell) const
    {
        std::map< index_t, char >::iterator iter =
            this_.cell_map.lower_bound(index);
        if(iter != this_.cell_map.end() && iter->first == index) {
            if(this_.base[index] == cell)
                this_.cell_map.erase(iter);
            else
                iter->second = cell;
        }
        else if(this_.base[index] != cell) {
            this_.cell_map.insert(iter, std::make_pair(index, cell));
        }
        return *this;
    }
};

/*******************************************************************************
 ******************************************************************************/

inline
delta_t::
delta_t(state_t const & base_, int)
    : base(base_)
{ }

inline delta_t&
delta_t::operator=(delta_t other)
{
    swap(other);
    return *this;
}

inline void
delta_t::
swap(delta_t& other)
{
    assert(&base == &other.base);
    using std::swap;
#define swap_( x ) swap(x, other.x)
    swap_(cell_map);
    swap_(robot_index);
    swap_(active_indices);
    swap_(n_turns);
    swap_(n_lambdas_remaining);
    swap_(robot_is_destroyed);
    swap_(n_turns_underwater);
    swap_(n_razors);
#undef swap_
}

inline int
delta_t::
score() const
{
    unsigned int const points_per_lambda =
        25 + (robot_index == base.lift_index ? 50 : !robot_is_destroyed ? 25 : 0);
    unsigned int const n_lambdas_collected =
        base.n_lambdas_collected
      + (base.n_lambdas_remaining - n_lambdas_remaining);
    return static_cast< int >(points_per_lambda * n_lambdas_collected)
         - static_cast< int >(n_turns);
}

inline unsigned int
delta_t::
water_level() const
{
    if(base.flooding_rate == 0)
        return base.water_level;
    unsigned int const orig_water_level =
        base.water_level + base.n_turns / base.flooding_rate;
    unsigned int const delta_water_level = n_turns / base.flooding_rate;
    return orig_water_level > delta_water_level ?
           orig_water_level - delta_water_level : 0;
}

inline delta_t::bracket_proxy
delta_t::
operator[](index_t const & index)
{ return bracket_proxy(*this, index); }

inline char
delta_t::
operator[](index_t const index) const
{
    std::map< index_t, char >::const_iterator iter = cell_map.find(index);
    return iter != cell_map.end() ? iter->second : base[index];
}

inline bool
delta_t::
move_is_valid(char const move) const
{ return icfp2012::move_is_valid(*this, move); }

inline bool
delta_t::
partial_equal(delta_t const & other) const
{
    return robot_index == other.robot_index
        && cell_map == other.cell_map;
}

inline bool
delta_t::
partial_less(delta_t const & other) const
{
    return n_turns <= other.n_turns
        && n_lambdas_remaining <= other.n_lambdas_remaining
        && !robot_is_destroyed
        && n_turns_underwater <= other.n_turns_underwater
        && n_razors >= other.n_razors;
}

inline bool
delta_t::
operator==(delta_t const & other) const
{
    return robot_index == other.robot_index
        && n_lambdas_remaining == other.n_lambdas_remaining
        && robot_is_destroyed == other.robot_is_destroyed
        && n_turns_underwater == other.n_turns_underwater
        && n_razors == other.n_razors
        && cell_map == other.cell_map;
}

inline bool
delta_t::
operator!=(delta_t const & other) const
{ return !operator==(other); }

inline std::size_t
delta_t::
hash_value() const
{ return boost::hash_range(cell_map.begin(), cell_map.end()); }

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCE_DELTA_T_HPP
