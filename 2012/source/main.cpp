/*******************************************************************************
 * icfp/2012/source/main.cpp
 *
 * Copyright 2012, Jeffrey Hellrung.
 * Distributed under the Boost Software License, Version 1.0.  (See accompanying
 * file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 ******************************************************************************/

#include <cassert>
#include <cstddef>
#include <cstdlib>

#include <deque>
#include <fstream>
#include <iostream>
#include <limits>

#include <boost/foreach.hpp>

#include "bfs_max_score.hpp"
#include "delta_t.hpp"
#include "dfs_bfs_max_score.hpp"
#include "index_t.hpp"
#include "state_t.hpp"

int main(int argc, char* argv[])
{
    using icfp2012::delta_t;
    using icfp2012::index_t;
    using icfp2012::state_t;

    state_t state;

    if(argc == 2) {
        // Interactive
        {
            std::ifstream f(argv[1]);
            if(f.fail()) {
                std::cerr << "Error opening file " << argv[1] << std::endl;
                return 1;
            }
            state.initialize(f);
        }

        std::cout << "Water: " << state.cells.size() - state.water_level << std::endl;
        std::cout << "Flooding: " << state.flooding_rate << std::endl;
        std::cout << "Waterproof: " << state.waterproof << std::endl;
        std::cout << "Growth: " << state.beard_growth_rate << std::endl;
        std::cout << "Razors: " << state.n_razors << std::endl;
        std::cout << state.trampoline_map
                  << state.target_map
                  << std::endl;

        std::cout << state << std::endl;

        state_t const base(state);
        delta_t delta(base);

        state_t simplified_state(state);
        simplified_state.simplify_ip();

        std::cout << simplified_state << std::endl;

        char move;
        while((std::cin >> move).good()) {
            if(move == 'A')
                break;
            bool const move_is_valid = state.move_is_valid(move);
            assert(move_is_valid == delta.move_is_valid(move));
            assert(move_is_valid == simplified_state.move_is_valid(move));
            if(!move_is_valid)
                move = 'W';

            state.move_robot_update_ip(move);

            delta = delta.move_robot_update(move);

            simplified_state.move_robot_update_ip(move);
            simplified_state.simplify_ip();

            bool const robot_is_destroyed = state.robot_is_destroyed;
            assert(robot_is_destroyed == delta.robot_is_destroyed);
            assert(robot_is_destroyed == simplified_state.robot_is_destroyed);

            if(!robot_is_destroyed) {
                for(std::size_t i = 0; i != state.cells.size(); ++i) {
                    for(std::size_t j = 0; j != state[i].size(); ++j) {
                        char const cell = state[i][j];
                        assert(cell == delta[index_t(i,j)]);
                        char const simplified_cell = simplified_state[i][j];
                        assert(cell == simplified_cell
                            || ((cell == '*' || cell == '@') && simplified_cell == '+')
                            || (cell == '.' && simplified_cell == ' '));
                    }
                }
                assert(state.active_indices.size() == delta.active_indices.size());
                assert(state.active_indices.size() == simplified_state.active_indices.size());
                int const score = state.score();
                assert(score == delta.score());
                assert(score == simplified_state.score());
                assert(state.water_level == delta.water_level());
#define assert_equal( member ) assert(state.member == delta.member)
                assert_equal(robot_index);
                assert_equal(n_turns);
                assert_equal(n_lambdas_remaining);
                assert_equal(n_turns_underwater);
                assert_equal(n_razors);
#undef assert_equal
#define assert_equal( member ) assert(state.member == simplified_state.member)
                assert_equal(robot_index);
                assert_equal(lift_index);
                assert_equal(n_turns);
                assert_equal(n_lambdas_remaining);
                assert_equal(n_lambdas_collected);
                assert_equal(water_level);
                assert_equal(n_turns_underwater);
                assert_equal(n_razors);
#undef assert_equal
                std::cout << simplified_state << std::endl;
                if(state.robot_index == state.lift_index)
                    break;
            }
            else {
                std::cout << "ROBOT DESTROYED!" << std::endl;
                break;
            }

        }

        std::cout << "# of Lambdas collected: " << state.n_lambdas_collected << std::endl;
        std::cout << "# of moves: " << state.n_turns << std::endl;
        std::cout << "Score: " << state.score() << std::endl;
    }
    else {
        enum strategy_e
        {
            strategy_e_bfs_max_score,
            strategy_e_dfs_bfs_max_score
        } strategy;
        std::size_t max_visited_states =
            std::numeric_limits< std::size_t >::max();
        if(argc > 2) {
            std::ifstream f(argv[1]);
            if(f.fail()) {
                std::cerr << "Error opening file " << argv[1] << std::endl;
                return 1;
            }
            std::string s(argv[2]);
            if(s == "bfs_max_score") {
                assert(argc == 3 || argc == 4);
                strategy = strategy_e_bfs_max_score;
                if(argc == 4)
                    max_visited_states =
                        static_cast< std::size_t >(std::atoi(argv[3]));
            }
            else if(s == "dfs_bfs_max_score") {
                assert(argc == 4);
                strategy = strategy_e_dfs_bfs_max_score;
                max_visited_states =
                        static_cast< std::size_t >(std::atoi(argv[3]));
            }
            else {
                std::cerr << "Unknown strategy parameter \"" << s << '"' << std::endl;
                return 1;
            }
            state.initialize(f);
        }
        else {
            assert(argc == 1);
            state.initialize(std::cin);
            strategy = strategy_e_bfs_max_score;
        }

        std::cout << "Water: " << state.cells.size() - state.water_level << std::endl;
        std::cout << "Flooding: " << state.flooding_rate << std::endl;
        std::cout << "Waterproof: " << state.waterproof << std::endl;
        std::cout << "Growth: " << state.beard_growth_rate << std::endl;
        std::cout << "Razors: " << state.n_razors << std::endl;
        std::cout << state.trampoline_map
                  << state.target_map
                  << std::endl;

        std::cout << state << std::endl;

        std::deque< char > path;
        switch(strategy) {
        case strategy_e_bfs_max_score:
            icfp2012::bfs_max_score(delta_t(state), path, max_visited_states);
            break;
        case strategy_e_dfs_bfs_max_score:
            icfp2012::dfs_bfs_max_score(delta_t(state), path, max_visited_states);
            break;
        }

        BOOST_FOREACH( char const move, path ) {
            std::cout << "Move: " << move << '\n' << std::endl;
            state.move_robot_update_ip(move);
            std::cout << state << std::endl;
        }

        std::cout << "# of Lambdas collected: " << state.n_lambdas_collected << std::endl;
        std::cout << "# of moves: " << state.n_turns << std::endl;
        std::cout << "Score: " << state.score() << std::endl;
    }
    return 0;
}
