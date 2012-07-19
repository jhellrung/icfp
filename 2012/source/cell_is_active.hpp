/*******************************************************************************
 * icfp/2012/source/cell_is_active.hpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_CELL_IS_ACTIVE_HPP
#define ICFP_2012_SOURCE_CELL_IS_ACTIVE_HPP

#include <cassert>
#include <cstddef>

#include "index_t.hpp"

namespace icfp2012
{

template< class State >
inline bool
rock_is_active(State const & state, index_t const index)
{
    index_t const indexD = index + 'D';
    char const cellD = state[indexD];
    bool const cellD_is_rock = cellD == '*' || cellD == '+' || cellD == '@';
    return cellD == ' '
        || ((cellD_is_rock || cellD == '\\')
         && ' ' == state[index + 'R'] && ' ' == state[indexD + 'R'])
        || (cellD_is_rock
         && ' ' == state[index + 'L'] && ' ' == state[indexD + 'L']);
}

template< class State >
inline bool
cell_is_active(State const & state, index_t const index)
{
    switch(state[index]) {
    case '*':
    case '@':
        return rock_is_active(state, index);
    case 'W':
        for(std::size_t i = index.i-1; i != index.i+2; ++i)
            for(std::size_t j = index.j-1; j != index.j+2; ++j)
                if(state[index_t(i,j)] == ' ')
                    return true;
    default:;
    }
    assert(state[index] != '+' || !rock_is_active(state, index));
    return false;
}

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCE_CELL_IS_ACTIVE_HPP
