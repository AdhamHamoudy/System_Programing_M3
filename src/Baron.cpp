// Email: adhamhamoudy3@gmail.com
#include "Baron.hpp"

using namespace std;

namespace coup {

Baron::Baron(Game& game, const string& name)
    : Player(game, name) {
    role_name = "Baron";
}

void Baron::invest() {
    validate_turn();
    if (coins() < 3) {
        throw runtime_error("Not enough coins to invest.");
    }
    remove_coins(3);
    add_coins(6);
    end_turn();
}

void Baron::on_sanctioned_by(Player& attacker) {
    (void)attacker;  // unused parameter
    add_coins(1);    // compensation for being sanctioned
}


}
