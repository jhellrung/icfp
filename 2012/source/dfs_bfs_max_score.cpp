/*******************************************************************************
 * icfp/2012/source/dfs_bfs_max_score.cpp
 *
 * Copyright 2012, Aleka McAdams, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#include <cassert>
#include <cstddef>

#include <deque>

#include <boost/foreach.hpp>

#include "bfs.hpp"
#include "delta_t.hpp"
#include "dfs_bfs_max_score.hpp"
#include "state_t.hpp"
#include "visited_state_t.hpp"
#include "visitor_result_e.hpp"

namespace icfp2012
{

namespace
{

struct visitor_t
{
    typedef visited_state_t<> visited_state_type;

    std::deque< char >& path;
    std::size_t n_visited_states;
    std::size_t const max_visited_states;
    int max_score0;
    int max_score;
    visited_state_type const * visited_with_max_score;

    visitor_t(
        std::deque< char >& path_,
        std::size_t const max_visited_states_)
        : path(path_),
          n_visited_states(0),
          max_visited_states(max_visited_states_),
          max_score0(1),
          max_score(0),
          visited_with_max_score(0)
    { }

    typedef visitor_result_e result_type;

    result_type operator()(visited_state_type& visited)
    {
        int const score0 = visited.state.score();
        if(score0 >= max_score0) {
            max_score0 = score0;
            std::deque< char > path1;
            int score;
            if(visited.state.cell_map.size() < visited.state.base.n_cells/64) {
                delta_t state1(visited.state);
                dfs_bfs_max_score(state1, path1, max_visited_states);
                BOOST_FOREACH( char const move, path1 )
                    state1 = state1.move_robot_update(move);
                score = state1.score();
            }
            else {
                state_t state1 = visited.state.apply();
                dfs_bfs_max_score(delta_t(state1), path1, max_visited_states);
                state1.move_robot_update_ip(path1);
                score = state1.score();
            }
            if(!visited_with_max_score || score > max_score) {
                max_score = score;
                visited_with_max_score = &visited;
                path.swap(path1);
            }
        }
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

void dfs_bfs_max_score(
    delta_t const & start,
    std::deque< char >& path,
    std::size_t const max_visited_states)
{ bfs(start, visitor_t(path, max_visited_states)); }

} // namespace icfp2012
