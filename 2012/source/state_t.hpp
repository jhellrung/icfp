/*******************************************************************************
 * icfp/2012/source/state_t.hpp
 *
 * Copyright 2012, Jeffrey Hellrung, Aleka McAdams.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_STATE_T_HPP
#define ICFP_2012_SOURCE_STATE_T_HPP

#include <cassert>
#include <cstddef>

#include <deque>
#include <iosfwd>
#include <vector>

#include <boost/foreach.hpp>

#include "index_t.hpp"
#include "move_is_valid.hpp"

namespace icfp2012
{

struct state_t
{
    std::vector< std::vector< char > > cells;
    std::size_t n_cells;

    index_t robot_index;
    index_t lift_index;
    std::deque< index_t > active_indices;

    unsigned int n_turns;
    unsigned int n_lambdas_remaining;
    unsigned int n_lambdas_collected;
    bool robot_is_destroyed;

    unsigned int water_level;
    unsigned int flooding_rate;
    unsigned int waterproof;
    unsigned int n_turns_underwater;

    unsigned int beard_growth_rate;
    unsigned int n_razors;

    struct trampoline_map_t
    {
        std::vector< index_t > targets;
        static std::size_t as_i(char const c);
        static char as_c(std::size_t const i);
        trampoline_map_t();
        index_t& operator[](char const c);
        index_t operator[](char const c) const;
    } trampoline_map;

    struct target_map_t
    {
        std::vector< std::vector< index_t > > trampolines;
        static std::size_t as_i(char const c);
        static char as_c(std::size_t const i);
        target_map_t();
        std::vector< index_t >& operator[](char const c);
        std::vector< index_t > const & operator[](char const c) const;
    } target_map;

    char& operator[](index_t const index);
    char operator[](index_t const index) const;
    std::vector< char >& operator[](std::size_t const i);
    std::vector< char > const & operator[](std::size_t const i) const;

    int score() const;

    void initialize(std::istream& is);

    bool move_is_valid(char const move) const;

    void simplify_ip();

    void move_robot_update_ip(char const move);
    void move_robot_update_ip(std::deque< char > const & moves);
};

std::ostream& operator<<(std::ostream& o, state_t const & this_);
std::ostream& operator<<(std::ostream& o, state_t::trampoline_map_t const & this_);
std::ostream& operator<<(std::ostream& o, state_t::target_map_t const & this_);

/*******************************************************************************
 ******************************************************************************/

inline char&
state_t::
operator[](index_t const index)
{ return cells[index.i][index.j]; }

inline char
state_t::
operator[](index_t const index) const
{ return cells[index.i][index.j]; }

inline std::vector< char >&
state_t::
operator[](std::size_t const i)
{ return cells[i]; }

inline std::vector< char > const &
state_t::
operator[](std::size_t const i) const
{ return cells[i]; }

inline int
state_t::
score() const
{
    unsigned int const points_per_lambda =
        25 + (robot_index == lift_index ? 50 : !robot_is_destroyed ? 25 : 0);
    return static_cast< int >(points_per_lambda * n_lambdas_collected)
         - static_cast< int >(n_turns);
}

inline bool
state_t::
move_is_valid(char const move) const
{ return icfp2012::move_is_valid(*this, move); }

inline void
state_t::
move_robot_update_ip(std::deque< char > const & moves)
{
    BOOST_FOREACH( char const move, moves )
        move_robot_update_ip(move);
}

/*******************************************************************************
 ******************************************************************************/

inline std::size_t
state_t::trampoline_map_t::as_i(char const c)
{
    assert('A' <= c && c <= 'I');
    return static_cast< std::size_t >(c - 'A');
}

inline char
state_t::trampoline_map_t::
as_c(std::size_t const i)
{
    assert(i < 9);
    return static_cast< char >(i) + 'A';
}

inline
state_t::trampoline_map_t::
trampoline_map_t()
    : targets(9)
{ }

inline index_t&
state_t::trampoline_map_t::
operator[](char const c)
{ return targets[as_i(c)]; }

inline index_t
state_t::trampoline_map_t::
operator[](char const c) const
{ return targets[as_i(c)]; }

/*******************************************************************************
 ******************************************************************************/

inline std::size_t
state_t::target_map_t::as_i(char const c)
{
    assert('1' <= c && c <= '9');
    return static_cast< std::size_t >(c - '1');
}

inline char
state_t::target_map_t::
as_c(std::size_t const i)
{
    assert(i < 9);
    return static_cast< char >(i) + '1';
}

inline
state_t::target_map_t::
target_map_t()
    : trampolines(9)
{ }

inline std::vector< index_t >&
state_t::target_map_t::
operator[](char const c)
{ return trampolines[as_i(c)]; }

inline std::vector< index_t > const &
state_t::target_map_t::
operator[](char const c) const
{ return trampolines[as_i(c)]; }

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCE_STATE_T_HPP
