/*******************************************************************************
 * icfp/2012/source/trampoline_map_t.cpp
 *
 * Copyright 2012, Jeffrey Hellrung, Aleka McAdams.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#include <cstddef>

#include <iostream>

#include "state_t.hpp"

namespace icfp2012
{

std::ostream&
operator<<(std::ostream& o, state_t::trampoline_map_t const & this_)
{
    for(std::size_t i = 0; i != this_.targets.size(); ++i) {
        index_t const index = this_.targets[i];
        if(index.valid())
            o << "Trampoline " << this_.as_c(i)
              << " targets " << index
              << std::endl;
    }
    return o;
};

} // namespace icfp2012
