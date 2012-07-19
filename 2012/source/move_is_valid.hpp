/*******************************************************************************
 * icfp/2012/source/move_is_valid.hpp
 *
 * Copyright 2012, Jeffrey Hellrung, Aleka McAdams.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_MOVE_IS_VALID_HPP
#define ICFP_2012_SOURCE_MOVE_IS_VALID_HPP

#include <cassert>

#include "index_t.hpp"

namespace icfp2012
{

template< class State >
inline bool
move_is_valid(State const & state, char const move)
{
    assert(state[state.robot_index] == 'R');
    if(move == 'W')
        return true;
    if(move == 'S'){
        if(state.n_razors == 0)
            return false;
        for(std::size_t i = state.robot_index.i-1; i != state.robot_index.i+2; ++i)
            for(std::size_t j = state.robot_index.j-1; j != state.robot_index.j+2; ++j)
                if(state[index_t(i,j)] == 'W')
                    return true;
        return false;
    }
    assert(move == 'L' || move == 'R' || move == 'U' || move == 'D');
    index_t const dest_index = state.robot_index + move;
    char const dest_cell = state[dest_index];
    if(dest_cell == '#')
        return false;
    if(dest_cell == ' '
    || dest_cell == '.'
    || dest_cell == '\\'
    || dest_cell == 'O'
    || dest_cell == '!')
        return true;
    if('A' <= dest_cell && dest_cell <= 'I')
        return true;
    if(dest_cell == '*' || dest_cell == '@')
        return (move == 'L' || move == 'R') && state[dest_index + move] == ' ';
    assert(dest_cell != '+' || !(move == 'L' || move == 'R') || state[dest_index + move] != ' ');
    return false;
}

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCE_MOVE_IS_VALID_HPP
