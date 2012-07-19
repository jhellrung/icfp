/*******************************************************************************
 * icfp/2012/source/update_rock.hpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_UPDATE_ROCK_HPP
#define ICFP_2012_SOURCE_UPDATE_ROCK_HPP

#include <cstddef>

#include "index_t.hpp"

namespace icfp2012
{

template< class State >
inline index_t
update_rock(State const & state, index_t const index)
{
    assert(state[index] == '*' || state[index] == '@');
    index_t const indexD = index + 'D';
    char const cellD = state[indexD];
    bool const cellD_is_rock = cellD == '*' || cellD == '+' || cellD == '@';
    if(cellD == ' ')
        return indexD;
    if((cellD_is_rock || cellD == '\\')
    && ' ' == state[index + 'R'] && ' ' == state[indexD + 'R'])
        return indexD + 'R';
    if(cellD_is_rock
    && ' ' == state[index + 'L'] && ' ' == state[indexD + 'L'])
        return indexD + 'L';
    return index;
}

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCE_UPDATE_ROCK_HPP
