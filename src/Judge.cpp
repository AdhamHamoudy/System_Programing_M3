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
    // No further action needed – bribe already paid and lost.
}

void Judge::on_sanctioned_by(Player& attacker) {
    attacker.remove_coins(1);  // Attacker pays extra
}

}
