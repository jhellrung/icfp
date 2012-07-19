/*******************************************************************************
 * icfp/2012/source/delta_t.cpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#include <cassert>
#include <cstddef>

#include <deque>
#include <set>
#include <utility>

#include <boost/foreach.hpp>
#include <boost/unordered_set.hpp>

#include "cell_is_active.hpp"
#include "delta_t.hpp"
#include "index_t.hpp"
#include "state_t.hpp"
#include "update_compare_t.hpp"
#include "update_rock.hpp"

namespace icfp2012
{

/*******************************************************************************
 * delta_t::delta_t(state_t const & base_)
 ******************************************************************************/

delta_t::
delta_t(state_t const & base_)
    : base(base_),
      robot_index(base.robot_index),
      active_indices(base.active_indices),
      n_turns(base.n_turns),
      n_lambdas_remaining(base.n_lambdas_remaining),
      robot_is_destroyed(base.robot_is_destroyed),
      n_turns_underwater(base.n_turns_underwater),
      n_razors(base.n_razors)
{ }

/*******************************************************************************
 * delta_t::move_robot_update(char const move) -> delta_t
 ******************************************************************************/

delta_t
delta_t::
move_robot_update(char const move) const
{
    assert(!robot_is_destroyed);
    assert(move_is_valid(move));

    delta_t result(base,0);
    result.cell_map = cell_map;
    result.robot_index = robot_index + move;
    result.n_turns = n_turns + 1;
    result.n_lambdas_remaining = n_lambdas_remaining;
    result.n_turns_underwater = n_turns_underwater;
    result.n_razors = n_razors;

    std::deque< index_t > new_empty_indices;

    // Move robot.
    bool rock_is_moved = false;
    switch(move) {
    case 'W':
        break;
    case 'S':
        if(n_razors == 0)
            break;
        --result.n_razors;
        for(std::size_t i = robot_index.i-1; i != robot_index.i+2; ++i) {
            for(std::size_t j = robot_index.j-1; j != robot_index.j+2; ++j) {
                index_t const index(i,j);
                if(operator[](index) != 'W')
                    continue;
                result[index] = ' ';
                new_empty_indices.push_back(index);
            }
        }
        break;
    default: {
        char const dest_cell = operator[](result.robot_index);
        switch(dest_cell) {
        case '*':
        case '@':
            assert(move == 'L' || move == 'R');
            assert(operator[](result.robot_index + move) == ' ');
            result[result.robot_index + move] = dest_cell;
            rock_is_moved = true;
            goto CASE_COMMON;
        case '\\':
            --result.n_lambdas_remaining;
            goto CASE_COMMON;
        case '!':
            ++result.n_razors;
            goto CASE_COMMON;
        case 'O':
        case '.':
        case ' ':
            goto CASE_COMMON;
        default:
            assert('A' <= dest_cell && dest_cell <= 'I');
            result.robot_index = base.trampoline_map[dest_cell];
            BOOST_FOREACH(
                index_t const trampoline_index,
                base.target_map[operator[](result.robot_index)] ) {
                result[trampoline_index] = ' ';
                new_empty_indices.push_back(trampoline_index);
            }
        CASE_COMMON:
            result[robot_index] = ' ';
            new_empty_indices.push_back(robot_index);
            result[result.robot_index] = 'R';
            break;
        }
    }}

    bool const grow_beards = base.beard_growth_rate != 0
                          && result.n_turns % base.beard_growth_rate == 0;

    boost::unordered_set< index_t > old_active_beards;

    // Determine which cells need updating.
    std::set< index_t, update_compare_t > update_srces;
    if(rock_is_moved) {
        index_t const index = result.robot_index + move;
        if(cell_is_active(result, index))
            update_srces.insert(index);
    }
    BOOST_FOREACH( index_t const index, active_indices ) {
        if(cell_is_active(result, index)) {
            if(grow_beards || result[index] != 'W')
                update_srces.insert(index);
            else
                old_active_beards.insert(index);
        }
    }
    BOOST_FOREACH( index_t const index, new_empty_indices ) {
        assert(result[index] == ' ');
        for(std::size_t i = index.i-1; i != index.i+2; ++i) {
            for(std::size_t j = index.j-1; j != index.j+2; ++j) {
                if(j >= base[i].size())
                    continue;
                index_t const adj_index(i,j);
                if(cell_is_active(result, adj_index)) {
                    if(grow_beards || result[adj_index] != 'W')
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
        assert(cell_is_active(result, index));
        char const cell = result[index];
        switch(cell) {
        case '*':
        case '@': {
            index_t const dest_index = update_rock(result, index);
            if(dest_index == index)
                break;
            update_dests.push_back(update_type(index, ' '));
            update_dests.push_back(update_type(dest_index,
                cell == '@' && result[dest_index + 'D'] != ' ' ? '\\' : cell));
            break;
        }
        case 'W':
            assert(grow_beards);
            for(std::size_t i = index.i-1; i != index.i+2; ++i) {
                for(std::size_t j = index.j-1; j != index.j+2; ++j) {
                    index_t const adj_index(i,j);
                    if(result[adj_index] == ' ')
                        update_dests.push_back(update_type(adj_index, 'W'));
                }
            }
            break;
        default:
            assert(false);
        }
    }
    update_srces.clear();

    // Apply updates.
    result.robot_is_destroyed = false;
    BOOST_FOREACH( update_type const update, update_dests ) {
        index_t const index = update.first;
        char const cell = update.second;
        result[index] = cell;
        if(result[index + 'D'] == 'R') {
            assert(cell != 'W');
            result.robot_is_destroyed = cell == '*' || cell == '\\';
        }
    }

    // Determine new set of active cells.
    boost::unordered_set< index_t > result_active_indices_hash;
    BOOST_FOREACH( index_t const index, old_active_beards )
        if(cell_is_active(result, index))
            result_active_indices_hash.insert(index);
    old_active_beards.clear();
    BOOST_FOREACH( update_type const update, update_dests ) {
        index_t const index = update.first;
        char const cell = update.second;
        assert(result[index] == cell);
        if(cell == ' ') {
            for(std::size_t i = index.i-1; i != index.i+2; ++i) {
                for(std::size_t j = index.j-1; j != index.j+2; ++j) {
                    if(j >= base[i].size())
                        continue;
                    index_t const adj_index(i,j);
                    if(cell_is_active(result, adj_index))
                        result_active_indices_hash.insert(adj_index);
                }
            }
        }
        else if(cell_is_active(result, index))
            result_active_indices_hash.insert(index);
    }
    update_dests.clear();
    result.active_indices.insert(
        result.active_indices.end(),
        result_active_indices_hash.begin(), result_active_indices_hash.end());

    // Check special conditions.
    if(result.robot_index == base.lift_index)
        result.robot_is_destroyed = false;
    else {
        if(result.n_lambdas_remaining == 0)
            result[base.lift_index] = 'O';
        if(result.robot_index.i < water_level())
            result.n_turns_underwater = 0;
        else if(++result.n_turns_underwater > base.waterproof)
            result.robot_is_destroyed = true;
    }

    return result;
}

} // namespace icfp2012
