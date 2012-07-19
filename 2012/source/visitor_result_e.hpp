/*******************************************************************************
 * icfp/2012/source/visitor_result_e.hpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_VISITOR_RESULT_E_HPP
#define ICFP_2012_SOURCE_VISITOR_RESULT_E_HPP

namespace icfp2012
{

enum visitor_result_e
{
    visitor_result_e_continue,
    visitor_result_e_skip,
    visitor_result_e_return
};

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCE_VISITOR_RESULT_E_HPP
