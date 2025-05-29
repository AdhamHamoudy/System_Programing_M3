// Email: adhamhamoudy3@gmail.com
#include "Judge.hpp"

using namespace std;

namespace coup {

Judge::Judge(Game& game, const string& name)
    : Player(game, name) {
    role_name = "Judge";
}

void Judge::cancel_bribe(Player& target) {
    if (!target.active()) {
        throw std::runtime_error("Target already eliminated.");
    }
    if (target.get_last_action() != "bribe") {
        throw std::runtime_error("Can only cancel a bribe action.");
    }
    if (target.coins() < 0) {
        throw std::runtime_error("Bribe already used or no effect.");
    }
    // No refund. Bribe is lost.
}


void Judge::on_sanctioned_by(Player& attacker) {
    attacker.remove_coins(1);  // Attacker pays extra
}

}
