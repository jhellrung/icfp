/*******************************************************************************
 * icfp/2012/source/bfs_max_score.cpp
 *
 * Copyright 2012, Aleka McAdams, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#include <cstddef>

#include <deque>

#include "bfs.hpp"
#include "bfs_max_score.hpp"
#include "visited_state_t.hpp"
#include "visitor_result_e.hpp"

namespace icfp2012
{

namespace
{

struct bfs_max_score_visitor_t
{
    std::deque< char >& path;
    std::size_t n_visited_states;
    std::size_t const max_visited_states;

    typedef visited_state_t<> visited_state_type;
    visited_state_type const * visited_with_max_score;

    bfs_max_score_visitor_t(
        std::deque< char >& path_,
        std::size_t const max_visited_states_)
        : path(path_),
          n_visited_states(0),
          max_visited_states(max_visited_states_),
          visited_with_max_score(0)
    { }

    typedef visitor_result_e result_type;

    result_type operator()(visited_state_type& visited)
    {
        if(!visited_with_max_score
        || visited_with_max_score->state.score() < visited.state.score())
            visited_with_max_score = &visited;
        if(++n_visited_states < max_visited_states
        && visited.state.robot_index != visited.state.base.lift_index)
            return visitor_result_e_continue;
        visited_state_type const * p = visited_with_max_score;
        while(p->parent) {
            assert(p->move);
            path.push_front(p->move);
            p = p->parent;
        }
        return visitor_result_e_return;
    }
};

} // namespace

void bfs_max_score(
    delta_t const & start,
    std::deque< char >& path,
    std::size_t const max_visited_states /*=
        std::numeric_limits< std::size_t >::max()*/)
{ bfs(start, bfs_max_score_visitor_t(path, max_visited_states)); }

} // namespace icfp2012
