/*******************************************************************************
 * icfp/2012/source/bfs.cpp
 *
 * Copyright 2012, Aleka McAdams, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

// #include <cstddef>

// #include <deque>
// #include <list>
// #include <utility>

// #include <boost/foreach.hpp>
// #include <boost/unordered_map.hpp>

// #include "delta.hpp"
#include "bfs.hpp"
// #include "state.hpp"

namespace icfp2012
{

namespace
{

struct visited_state
{
    delta data;
    visited_state const * parent;
    char c;
    bool active;
    int alternative_score;
    visited_state(
        delta const & data_,
        visited_state const * const parent_,
        char const c_, int alternative_score_)
        : data(data_), parent(parent_), c(c_), active(true),
         alternative_score(alternative_score_)
    { }
};

} // namespace

void path_with_highest_running_score(
    icfp2012::delta const & dx,
    std::deque< char >& path,
    std::size_t max_deltas,
    std::deque< char >* alternative_path1,
    int* alternative_high_score)
{
    typedef std::list< visited_state > visited_sublist_type;
    typedef boost::unordered_map<
        std::size_t, visited_sublist_type
    > visited_states_type;

    visited_states_type visited_states;
    std::deque< visited_state const * > q;
    visited_states_type::iterator iter;
    {
        delta delta_x(dx);
        iter = visited_states.emplace(
            delta_x.hash_value(), visited_sublist_type()).first;
        iter->second.push_back(visited_state(delta_x,0,0,0));
    }
    q.push_back(&iter->second.back());

    int high_score = dx.score();
    int alt_high_score = dx.score();

    unsigned int longest_path = 0;
    unsigned int total_deltas = 0;
    const visited_state * best_visited_state = &iter->second.back();
    const visited_state * alt_best_visited_state = &iter->second.back();

    while(!q.empty()) {
        visited_state const * p = q.front();
        q.pop_front();
        if(!p->active)
            continue;
        static char const moves[] = { 'U', 'L', 'R', 'D', 'W', 'S' };
        for(std::size_t i = 0; i != sizeof( moves ); ++i) {
            char c = moves[i];
            if(!p->data.move_is_valid(c))
                continue;

            int alt_score = 25;
            delta const d = p->data.move_robot_update(c);
            if(p->data.n_razors < d.n_razors)
                alt_score = 25;
            if(d.robot_is_destroyed())
                continue;
            
            if(d.n_turns > longest_path)
                longest_path = d.n_turns;
            
            {iter = visited_states.emplace(
                d.hash_value(), visited_sublist_type()).first;
            visited_sublist_type& visited_sublist = iter->second;
            if(!visited_sublist.empty()) {
                visited_sublist_type::iterator jter = visited_sublist.begin();
                for(; jter != visited_sublist.end(); ++jter) {
                    delta const & e = jter->data;
                    if(e.partial_equal(d) && e.partial_less(d))
                        goto FOR_I_CONTINUE;
                }
                while(jter != visited_sublist.begin()) {
                    delta& e = (--jter)->data;
                    if(e.n_turns < d.n_turns)
                        break;
                    if(e.partial_equal(d) && !e.partial_less(d) && d.partial_less(e)){
                        --total_deltas;
                        jter->active = false;
                    }
                }
            }
            ++total_deltas;
            int score = d.score();
            alt_score += score-p->data.score()+p->alternative_score;
            visited_sublist.push_back(visited_state(d,p,c,alt_score));
            q.push_back(&visited_sublist.back());

            if(score > high_score){
#ifndef FINAL_SUBMISSION
                std::cout<<"New high score: "<<score<<std::endl;
#endif
                high_score = score;
                best_visited_state = &visited_sublist.back();
            }if(alt_score > alt_high_score){
#ifndef FINAL_SUBMISSION
                std::cout<<"New alt high score: "<<alt_score<<std::endl;
#endif
                alt_high_score = alt_score;
                alt_best_visited_state = &visited_sublist.back();
            }
            }FOR_I_CONTINUE:;
            
            if(d.robot_ij == d.base.lift_ij || icfp2012::signal_received == true || total_deltas >= max_deltas) {
                goto EXIT_EARLY;
            }
        }        
    }
  EXIT_EARLY:
#ifndef FINAL_SUBMISSION
    std::cout<<"# of deltas: "<<total_deltas<<std::endl;
    std::cout<<"Longest path: "<<longest_path<<std::endl;
    std::cout<<"Best score: "<<high_score<<std::endl;
    std::cout<<"Alt best score: "<<high_score<<std::endl;
#endif
    visited_state const* p = best_visited_state->parent;
    if(p){
        char c = best_visited_state->c;
        do {
            path.push_front(c);
            c = p->c;
        } while((p=p->parent));
    }
   
    if(alternative_path1){
        visited_state const* p = alt_best_visited_state->parent;
        if(p){
            char c = alt_best_visited_state->c;
            alternative_path1->push_front(c);
            c = p->c;
        }while((p=p->parent));
    }
    if(alternative_high_score)
        *alternative_high_score = alt_high_score;
}

}
