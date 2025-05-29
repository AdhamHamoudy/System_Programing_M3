//Email:adhamhamoudy3@gmail.com
#include "Game.hpp"
#include "Player.hpp"
#include "General.hpp"
using namespace std;

namespace coup {

Game::Game() {}

void Game::add_player(Player* player) {
    if (active_players.size() >= 6) {
        throw runtime_error("Maximum number of players (6) reached.");
    }
    active_players.push_back(player);
}

vector<string> Game::players() const {
    vector<string> names;
    for (Player* p : active_players) {
        names.push_back(p->name());
    }
    return names;
}

string Game::turn() const {
    if (active_players.empty()) {
        throw runtime_error("No players in game.");
    }
    return active_players.at(current_turn_index)->name();
}

string Game::winner() const {
    if (active_players.size() == 1) {
        return active_players.at(0)->name();
    }
    if (active_players.size() < 1) {
        throw runtime_error("Game has not started or not enough players.");
    }
    throw runtime_error("Game is still in progress. No winner yet.");
}



void Game::eliminate(Player* player) {
    for (size_t i = 0; i < active_players.size(); ++i) {
        if (active_players[i] == player) {
            active_players.erase(active_players.begin() + i);
            if (i < current_turn_index && current_turn_index > 0) {
                --current_turn_index;  // adjust turn index if needed
            }
            if (current_turn_index >= active_players.size()) {
                current_turn_index = 0;
            }
            return;
        }
    }
    throw runtime_error("Player not found in active list.");
}

void Game::advance_turn() {
    if (active_players.empty()) return;
    current_turn_index = (current_turn_index + 1) % active_players.size();
    // Future improvement: trigger start_turn if needed
    // active_players[current_turn_index]->start_turn();
}

void Game::coup(Player* attacker, Player* target) {
    if (attacker->coins() < 7) {
        throw runtime_error("Not enough coins to perform a coup.");
    }

    attacker->remove_coins(7);

    // Check if target is a General and can block
    if (target->role() == "General") {
        General* general = dynamic_cast<General*>(target);
        if (general && general->coins() >= 5) {
            general->block_coup(*general);  // sets target.coup_blocked = true
        }
    }

    if (!target->is_coup_blocked()) {
        target->eliminate();
    } else {
        target->set_coup_blocked(false);  // reset the flag
    }

    advance_turn();
}

} // namespace coup