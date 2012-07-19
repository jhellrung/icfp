/*******************************************************************************
 * icfp/2012/source/visited_state_t.hpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_VISITED_STATE_T_HPP
#define ICFP_2012_SOURCE_VISITED_STATE_T_HPP

#include "delta_t.hpp"

namespace icfp2012
{

template< class Data = void >
struct visited_state_t
{
    delta_t state;
    visited_state_t const * parent;
    char move;
    bool active;
    Data data;
    visited_state_t(
        delta_t const & state_,
        visited_state_t const * const parent_ = 0,
        char const move_ = 0)
        : state(state_),
          parent(parent_),
          move(move_),
          active(true)
    { }
};

template<>
struct visited_state_t< void >
{
    delta_t state;
    visited_state_t const * parent;
    char move;
    bool active;
    visited_state_t(
        delta_t const & state_,
        visited_state_t const * const parent_ = 0,
        char const move_ = 0)
        : state(state_),
          parent(parent_),
          move(move_),
          active(true)
    { }
};

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCE_VISITED_STATE_T_HPP
