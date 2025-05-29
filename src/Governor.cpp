// Email: adhamhamoudy3@gmail.com
#include "Governor.hpp"

using namespace std;

namespace coup {

Governor::Governor(Game& game, const string& name)
    : Player(game, name) {
    this->role_name = "Governor";
}

void Governor::tax() {
    validate_turn();
    if (under_sanction) {
        throw runtime_error("You are under sanction and cannot tax.");
    }
    add_coins(3); // Governor gets 3 coins instead of 2
    end_turn();
}

void Governor::undo(Player& other) {
    if (other.get_last_action() != "tax") {
        throw std::runtime_error("Governor can only undo a tax action.");
    }    
    // Governor can cancel the tax effect of another player
    if (!other.active()) {
        throw runtime_error("Cannot undo a dead player.");
    }
    if (other.coins() < 2) {
        throw runtime_error("Cannot undo: player does not have enough coins to remove.");
    }
    other.remove_coins(2);
}

}
