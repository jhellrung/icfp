/*******************************************************************************
 * icfp/2012/source/dfs_bfs_max_score.cpp
 *
 * Copyright 2012, Aleka McAdams, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#include <cassert>
#include <cstddef>

#include <algorithm>
#include <deque>
#include <vector>

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
    std::size_t const max_visited_states;
    std::size_t const max_branches;

    int base_score;
    std::size_t n_visited_states;
    std::vector< visited_state_type const * > visited_with_max_scores;

    visitor_t(
        std::deque< char >& path_,
        std::size_t const max_visited_states_,
        std::size_t const max_branches_)
        : path(path_),
          max_visited_states(max_visited_states_),
          max_branches(max_branches_),
          n_visited_states(0)
    { visited_with_max_scores.reserve(max_branches); }

private:
    struct compare_visited_scores
    {
        typedef bool result_type;
        bool operator()(
            visited_state_type const * p0,
            visited_state_type const * p1) const
        { return p1->state.score() < p0->state.score(); }
    };
public:

    typedef visitor_result_e result_type;

    result_type operator()(visited_state_type& visited)
    {
        int score = visited.state.score();

        if(!visited.parent) {
            base_score = score;
            return visitor_result_e_continue;
        }

        if(score > base_score
        && (visited_with_max_scores.size() < max_branches
         || score > visited_with_max_scores.back()->state.score())) {
            if(visited_with_max_scores.size() < max_branches)
                visited_with_max_scores.push_back(&visited);
            else
                visited_with_max_scores.back() = &visited;
            std::inplace_merge(
                visited_with_max_scores.begin(),
                visited_with_max_scores.end() - 1,
                visited_with_max_scores.end(),
                compare_visited_scores()
            );
        }

        if(++n_visited_states < max_visited_states
        && visited.state.robot_index != visited.state.base.lift_index)
            return visitor_result_e_continue;

        if(!visited_with_max_scores.empty()) {
            int max_score = 0;
            visited_state_type const * p = 0;
            BOOST_FOREACH(
                visited_state_type const * q,
                visited_with_max_scores ) {
                std::deque< char > path1;
                if(q->state.robot_index == q->state.base.lift_index) {
                    score = q->state.score();
                }
                else {
                    state_t state1 = q->state.apply();
                    state1.simplify_ip();
                    dfs_bfs_max_score(
                        delta_t(state1), path1,
                        max_visited_states, max_branches);
                    state1.move_robot_update_ip(path1);
                    score = state1.score();
                }
                if(score > max_score) {
                    max_score = score;
                    p = q;
                    path.swap(path1);
                }
            }
            assert(p);
            do {
                assert(p->move);
                path.push_front(p->move);
            } while((p = p->parent)->parent);
        }

        return visitor_result_e_return;
    }
};

} // namespace

void dfs_bfs_max_score(
    delta_t const & start,
    std::deque< char >& path,
    std::size_t const max_visited_states,
    std::size_t const max_branches)
{ bfs(start, visitor_t(path, max_visited_states, max_branches)); }

} // namespace icfp2012
