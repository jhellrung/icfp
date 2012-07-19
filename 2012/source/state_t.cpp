/*******************************************************************************
 * icfp/2012/source/state_t.cpp
 *
 * Copyright 2012, Jeffrey Hellrung, Aleka McAdams.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#include <cassert>
#include <cstddef>

#include <deque>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>

#include "cell_is_active.hpp"
#include "index_t.hpp"
#include "state_t.hpp"
#include "update_compare_t.hpp"
#include "update_rock.hpp"

namespace icfp2012
{

namespace
{

inline bool
is_unmovable(char const cell)
{ return cell == '#' || cell == '+' || cell == 'L' || cell == 'O'; }

} // namespace

/*******************************************************************************
 * state_t::initialize(std::istream& is) -> void
 ******************************************************************************/

void
state_t::
initialize(std::istream& is)
{
    n_turns = 0;
    n_lambdas_remaining = 0;
    n_lambdas_collected = 0;
    robot_is_destroyed = false;

    flooding_rate = 0;
    waterproof = 10;
    n_turns_underwater = 0;

    beard_growth_rate = 25;
    n_razors = 0;

    std::string s;

    while(std::getline(is, s).good() && !s.empty()) {
        cells.push_back(std::vector< char >());
        cells.back().reserve(s.size());
        cells.back().insert(cells.back().end(), s.begin(), s.end());
    }

    water_level = cells.size();

    std::vector< std::size_t > trampoline_map_targets(9);
    std::vector< std::vector< std::size_t > > target_map_trampolines(9);

    while(is.good()) {
        s.clear();
        is >> s;
        if(s == "Water") {
            unsigned int i;
            is >> i;
            water_level -= i;
        }
        else if(s == "Flooding")
            is >> flooding_rate;
        else if(s == "Waterproof")
            is >> waterproof;
        else if(s == "Trampoline") {
            char c;
            is >> c;
            std::size_t trampoline = trampoline_map_t::as_i(c);
            is >> s >> c;
            assert(s == "targets");
            std::size_t target = target_map_t::as_i(c);
            trampoline_map_targets[trampoline] = target;
            target_map_trampolines[target].push_back(trampoline);
        }
        else if(s == "Growth")
            is >> beard_growth_rate;
        else if(s == "Razors")
            is >> n_razors;
        else
            assert(s.empty());
    }

    bool robot_found = false;
    bool lift_found = false;

    for(std::size_t i = 0; i != cells.size(); ++i) {
        for(std::size_t j = 0; j != cells[i].size(); ++j) {
            index_t index(i,j);
            char const cell = cells[i][j];
            switch(cell) {
            case 'R':
                assert(!robot_found);
                robot_index = index;
                robot_found = true;
                break;
            case 'L':
                assert(!lift_found);
                lift_index = index;
                lift_found = true;
                break;
            case '\\':
                ++n_lambdas_remaining;
                break;
            case '@':
                ++n_lambdas_remaining;
            case '*':
            case 'W':
                if(cell_is_active(*this, index))
                    active_indices.push_back(index);
                break;
            default:
                if('A' <= cell && cell <= 'I') {
                    std::size_t const trampoline = trampoline_map_t::as_i(cell);
                    std::size_t const target = trampoline_map_targets[trampoline];
                    target_map[target].push_back(index);
                }
                else if('1' <= cell && cell <= '9') {
                    std::size_t const target = target_map_t::as_i(cell);
                    BOOST_FOREACH( std::size_t const trampoline, target_map_trampolines[target] )
                        trampoline_map[trampoline] = index;
                }
            }
        }
    }

    assert(robot_found);
    assert(lift_found);
}

/*******************************************************************************
 * state_t::simplify() -> void
 ******************************************************************************/

void
state_t::
simplify_ip()
{
    // Identify unmovable rocks ('+').
    bool beard_found = false;
    std::size_t n_cols = 0;
    for(std::size_t i = cells.size() - 2; i != 0; --i) {
        if(n_cols < cells[i].size())
            n_cols = cells[i].size();
        for(std::size_t j = 0; j != cells[i].size(); ++j) {
            char& cell = cells[i][j];
            if(cell == 'W')
                beard_found = true;
            if(!(cell == '*' || cell == '@'))
                continue;
            char const cellD = cells[i+1][j];
            if(!is_unmovable(cellD))
                continue;
            assert(0 < j && j < cells[i].size() - 1);
            if(cellD == '+') {
                if((is_unmovable(cells[i][j-1]) || is_unmovable(cells[i+1][j-1]))
                && (is_unmovable(cells[i][j+1]) || is_unmovable(cells[i+1][j+1])))
                    cell = '+';
            }
            else if(is_unmovable(cells[i][j-1]))
                cell = '+';
            else {
                char const cellR = cells[i][j+1];
                if(is_unmovable(cellR))
                    cell = '+';
                else if((cellR == '*' || cellR == '@') && is_unmovable(cells[i+1][j+1]))
                    cell = '+';
            }
        }
    }

    if(beard_found)
        return;

    // Identify earth ('.') which may be safely set to empty space (' ').
    std::vector< bool > earth_mask(n_cols, false);
    for(std::size_t i = 0; i != cells.size(); ++i) {
        bool b = false;
        for(std::size_t j = 0; j != cells[i].size(); ++j) {
            char const cell = cells[i][j];
            if(is_unmovable(cell)) {
                earth_mask[j] = b = false;
                continue;
            }
            if(b) {
                earth_mask[j] = true;
                continue;
            }
            if(!earth_mask[j]) {
                if(!(cell == '*' || cell == '@'))
                    continue;
                earth_mask[j] = true;
            }
            b = true;
            for(std::size_t k = j; k != 0;) {
                --k;
                if(earth_mask[k] || is_unmovable(cells[i][k]))
                    break;
                earth_mask[k] = true;
            }
        }
        for(std::size_t j = 0; j != cells[i].size(); ++j) {
            char& cell = cells[i][j];
            if(cell == '.' && !earth_mask[j])
                cell = ' ';
        }
    }
}

/*******************************************************************************
 * state_t::move_robot_update(char const move) -> void
 ******************************************************************************/

void
state_t::
move_robot_update_ip(char const move)
{
    assert(!robot_is_destroyed);
    assert(move != 'A');

    ++n_turns;

    std::deque< index_t > new_empty_indices;

    // Move robot.
    bool rock_is_moved = false;
    switch(move) {
    case 'W':
        break;
    case 'S':
        if(n_razors == 0)
            break;
        --n_razors;
        for(std::size_t i = robot_index.i-1; i != robot_index.i+2; ++i) {
            for(std::size_t j = robot_index.j-1; j != robot_index.j+2; ++j) {
                if(cells[i][j] != 'W')
                    continue;
                cells[i][j] = ' ';
                new_empty_indices.push_back(index_t(i,j));
            }
        }
        break;
    default: {
        index_t dest_index = robot_index + move;
        char const dest_cell = operator[](dest_index);
        switch(dest_cell) {
        case '*':
        case '@': {
            char& other_cell = operator[](dest_index + move);
            if(!(move == 'L' || move == 'R') || other_cell != ' ')
                break;
            other_cell = dest_cell;
            rock_is_moved = true;
            goto CASE_COMMON;
        }
        case '\\':
            ++n_lambdas_collected;
            --n_lambdas_remaining;
            goto CASE_COMMON;
        case '!':
            ++n_razors;
            goto CASE_COMMON;
        case 'O':
        case '.':
        case ' ':
            goto CASE_COMMON;
        default:
            if(!('A' <= dest_cell && dest_cell <= 'I'))
                break;
            dest_index = trampoline_map[dest_cell];
            BOOST_FOREACH(
                index_t const trampoline_index,
                target_map[operator[](dest_index)] ) {
                operator[](trampoline_index) = ' ';
                new_empty_indices.push_back(trampoline_index);
            }
        CASE_COMMON:
            operator[](robot_index) = ' ';
            new_empty_indices.push_back(robot_index);
            operator[](dest_index) = 'R';
            robot_index = dest_index;
        }
    }}

    if(flooding_rate != 0 && (n_turns % flooding_rate) == 0 && water_level != 0)
        --water_level;

    bool const grow_beards = beard_growth_rate != 0
                          && (n_turns % beard_growth_rate == 0);

    boost::unordered_set< index_t > old_active_beards;

    // Determine which cells need updating.
    std::set< index_t, update_compare_t > update_srces;
    if(rock_is_moved) {
        index_t const index = robot_index + move;
        if(cell_is_active(*this, index))
            update_srces.insert(index);
    }
    BOOST_FOREACH( index_t const index, active_indices) {
        if(cell_is_active(*this, index)) {
            if(grow_beards || operator[](index) != 'W')
                update_srces.insert(index);
            else
                old_active_beards.insert(index);
        }
    }
    active_indices.clear();
    BOOST_FOREACH( index_t const index, new_empty_indices) {
        assert(operator[](index) == ' ');
        for(std::size_t i = index.i-1; i != index.i+2; ++i) {
            for(std::size_t j = index.j-1; j != index.j+2; ++j) {
                if(j >= cells[i].size())
                    continue;
                index_t const adj_index(i,j);
                if(cell_is_active(*this, adj_index)){
                    if(grow_beards || operator[](adj_index) != 'W')
                        update_srces.insert(adj_index);
                    else
                        old_active_beards.insert(adj_index);
                }
            }
        }
    }

    // Determine how cells should be updated.
    typedef std::pair< index_t, char > update_type;
    std::deque< update_type > update_dests;
    BOOST_FOREACH( index_t const index, update_srces ) {
        assert(cell_is_active(*this, index));
        char const cell = operator[](index);
        switch(cell) {
        case '*':
        case '@': {
            index_t const dest_index = update_rock(*this, index);
            if(dest_index == index)
                break;
            update_dests.push_back(update_type(index, ' '));
            update_dests.push_back(update_type(dest_index,
                cell == '@' && operator[](dest_index + 'D') != ' ' ? '\\' : cell));
            break;
        }
        case 'W':
            assert(grow_beards);
            for(std::size_t i = index.i-1; i != index.i+2; ++i)
                for(std::size_t j= index.j-1; j != index.j+2; ++j)
                    if(cells[i][j] == ' ')
                        update_dests.push_back(update_type(index_t(i,j), 'W'));
            break;
        default:
            assert(false);
        }
    }
    update_srces.clear();

    // Apply updates.
    robot_is_destroyed = false;
    BOOST_FOREACH( update_type const update, update_dests ) {
        index_t const index = update.first;
        char const cell = update.second;
        operator[](index) = cell;
        if(operator[](index + 'D') == 'R') {
            assert(cell != '@');
            robot_is_destroyed = cell == '*' || cell == '\\';
        }
    }

    // Determine new set of active cells.
    boost::unordered_set< index_t > active_indices_hash;
    BOOST_FOREACH( index_t const index, old_active_beards )
        if(cell_is_active(*this, index))
            active_indices_hash.insert(index);
    old_active_beards.clear();
    BOOST_FOREACH( update_type const update, update_dests ) {
        index_t const index = update.first;
        char const cell = update.second;
        assert(operator[](index) == cell);
        if(cell == ' '){
            for(std::size_t i = index.i-1; i != index.i+2; ++i) {
                for(std::size_t j = index.j-1; j != index.j+2; ++j) {
                    if(j >= cells[i].size())
                        continue;
                    index_t const adj_index(i,j);
                    if(cell_is_active(*this, adj_index))
                        active_indices_hash.insert(adj_index);
                }
            }
        }
        else if(cell_is_active(*this, index))
            active_indices_hash.insert(index);
    }
    update_dests.clear();
    active_indices.insert(
        active_indices.end(),
        active_indices_hash.begin(), active_indices_hash.end());

    // Check special conditions.
    if(robot_index == lift_index)
        robot_is_destroyed = false;
    else {
        if(n_lambdas_remaining == 0)
            operator[](lift_index) = 'O';
        if(robot_index.i < water_level)
            n_turns_underwater = 0;
        else if(++n_turns_underwater > waterproof)
            robot_is_destroyed = true;
    }
}

/*******************************************************************************
 * operator<<(std::ostream& o, char const move) -> std::ostream&
 ******************************************************************************/

std::ostream&
operator<<(std::ostream& o, state_t const & this_)
{
    o << "After " << this_.n_turns << " turns: " << std::endl;
    o << "Turns underwater: " << this_.n_turns_underwater << std::endl;

    for(std::size_t i = 0; i != this_.cells.size(); ++i) {
        if(i == this_.water_level) {
            for(std::size_t j = 0; j != this_.cells[i].size(); ++j)
                o << '~';
            o << std::endl;
        }
        for(std::size_t j = 0; j != this_.cells[i].size(); ++j)
            o << this_[i][j];
        o << std::endl;
    }
    return o;
}

} // namespace icfp2012
