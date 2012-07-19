/*******************************************************************************
 * icfp/2012/source/bfs_max_score.hpp
 *
 * Copyright 2012, Aleka McAdams, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_BFS_MAX_SCORE_HPP
#define ICFP_2012_SOURCE_BFS_MAX_SCORE_HPP

#include <cstddef>

#include <deque>
#include <limits>

#include "delta_t.hpp"

namespace icfp2012
{

void bfs_max_score(
    delta_t const & start,
    std::deque< char >& path,
    std::size_t const max_visited_states =
        std::numeric_limits< std::size_t >::max());

} // namespace icfp2012

#endif // #define ICFP_2012_SOURCE_BFS_MAX_SCORE_HPP
