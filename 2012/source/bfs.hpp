/*******************************************************************************
 * icfp/2012/source/bfs.hpp
 *
 * Copyright 2012, Aleka McAdams, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_BFS_HPP
#define ICFP_2012_SOURCE_BFS_HPP

#include <cstddef>

#include <deque>
#include <list>

#include <boost/unordered_map.hpp>

#include "delta_t.hpp"
#include "visitor_result_e.hpp"
#include "visited_state_t.hpp"

namespace icfp2012
{

template< class Data, class Visitor >
void bfs(delta_t const & start, Visitor visitor)
{
    typedef visited_state_t< Data > visited_state_type;
    typedef std::list< visited_state_type > visited_sublist_type;
    typedef boost::unordered_map<
        std::size_t, visited_sublist_type
    > visited_states_type;

    visited_states_type visited_states;
    std::deque< visited_state_type const * > q;
    visited_states_type::iterator iter = visited_states.emplace(
        start.hash_value(), visited_sublist_type()).first;
    iter->second.push_back(visited_state_type(start));
    visitor(iter->second.back());
    q.push_back(&iter->second.back());

    while(!q.empty()) {
        visited_state_type const * current = q.front();
        q.pop_front();
        if(!current->active)
            continue;

        static char const moves[] = { 'L', 'R', 'U', 'D', 'S', 'W' };
        for(std::size_t i = 0; i != sizeof( moves ); ++i) {{
            char const move = moves[i];
            if(!current->state.move_is_valid(move))
                continue;
            delta_t const next = current->state.move_robot_update(move);
            if(next.robot_is_destroyed)
                continue;
            iter = visited_states.emplace(
                next.hash_value(), visited_sublist_type()).first;
            visited_sublist_type& visited_sublist = iter->second;
            visited_sublist_type::iterator jter = visited_sublist.begin();
            for(; jter != visited_sublist.end(); ++jter) {
                delta_t const & visited = jter->state;
                if(visited.partial_equal(next)
                && visited.partial_less(next))
                    goto FOR_I_CONTINUE;
            }
            while(jter != visited_sublist.begin()) {
                delta_t const & visited = (--jter)->state;
                if(visited.n_turns < next.n_turns)
                    break;
                if(next.partial_equal(visited)
                && next.partial_less(visited)
                && !visited.partial_less(next))
                    jter->active = false;
            }
            visited_sublist.push_back(visited_state_type(next, current, move));
            switch(visitor(visited_sublist.back())) {
            case visitor_result_e_continue:
                q.push_back(&visited_sublist.back());
                break;
            case visitor_result_e_skip:
                visited_sublist.pop_back();
                break;
            case visitor_result_e_return:
                return;
            }
        }FOR_I_CONTINUE:;}

    }
}

template< class Visitor >
inline void bfs(delta_t const & start, Visitor const & visitor)
{ bfs< void >(start, visitor); }

} // namespace icfp2012

#endif // #define ICFP_2012_SOURCE_BFS_HPP
