/*******************************************************************************
 * icfp/2012/source/index_t.hpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#ifndef ICFP_2012_SOURCE_INDEX_T_HPP
#define ICFP_2012_SOURCE_INDEX_T_HPP

#include <cstddef>

#include <iostream>

#include <boost/functional/hash.hpp>

namespace icfp2012
{

struct index_t
{
    std::size_t i;
    std::size_t j;

    static int as_i(char const c)
    { return (c == 'D') - (c == 'U'); }
    static int as_j(char const c)
    { return (c == 'R') - (c == 'L'); }

    index_t(std::size_t const i_ = -1, std::size_t const j_ = -1)
        : i(i_), j(j_)
    { }

    bool valid() const
    { return i != -1 && j != -1; }

    void assign(std::size_t const i_ = -1, std::size_t const j_ = -1)
    {
        i = i_;
        j = j_;
    }

    bool operator==(index_t const & other) const
    { return i == other.i && j == other.j; }
    bool operator!=(index_t const & other) const
    { return !operator==(other); }

    bool operator<(index_t const & other) const
    { return i < other.i || i == other.i && j < other.j; }
    bool operator>(index_t const & other) const
    { return other < *this; }

    bool operator<=(index_t const & other) const
    { return !(other < *this); }
    bool operator>=(index_t const & other) const
    { return !(*this < other); }

    index_t& operator+=(char const c)
    {
        i += as_i(c);
        j += as_j(c);
        return *this;
    }

    index_t operator+(char const c) const
    { return index_t(i + as_i(c), j + as_j(c)); }
    inline friend index_t
    operator+(char const c, index_t const & this_)
    { return this_ + c; }

    index_t& operator-=(char const c)
    {
        i -= as_i(c);
        j -= as_j(c);
        return *this;
    }

    index_t operator-(char const c) const
    { return index_t(i - as_i(c), j - as_j(c)); }

    std::size_t hash_value() const
    {
        std::size_t result = boost::hash< std::size_t >()(i);
        boost::hash_combine(result, j);
        return result;
    }
    inline friend std::size_t
    hash_value(index_t const & this_)
    { return this_.hash_value(); }

    inline friend std::ostream&
    operator<<(std::ostream& o, index_t const & this_)
    { return o << '(' << this_.i << ',' << this_.j << ')'; }
};

} // namespace icfp2012

#endif // #ifndef ICFP_2012_SOURCE_INDEX_T_HPP
