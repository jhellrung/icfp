/*******************************************************************************
 * icfp/2012/sources/update_compare_t.hpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCES_UPDATE_COMPARE_T_HPP
#define ICFP_2012_SOURCES_UPDATE_COMPARE_T_HPP

#include "index_t.hpp"

namespace icfp2012
{

struct update_compare_t
{
    typedef bool result_type;
    bool operator()(index_t const index0, index_t const index1) const
    { return index0.i > index1.i || index0.i == index1.i && index0.j < index1.j; }
};

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCES_UPDATE_COMPARE_T_HPP
