// Email: adhamhamoudy3@gmail.com
#include "Merchant.hpp"

using namespace std;

namespace coup {

Merchant::Merchant(Game& game, const string& name)
    : Player(game, name) {
    role_name = "Merchant";
}

void Merchant::start_turn_bonus() {
    if (coins() >= 3) {
        add_coins(1);  // Gets 1 free coin if starting turn with 3 or more
    }
}

void Merchant::on_arrested_by(Player& by) {
    (void)by;
    if (coins() >= 2) {
        remove_coins(2);  // Pays 2 coins to the bank
    } else {
        remove_coins(coins());  // Removes all coins if less than 2
    }
}

void Merchant::validate_turn() const {
    Player::validate_turn();
    // Grants +1 coin bonus if 3 or more coins at start of turn
    const_cast<Merchant*>(this)->start_turn_bonus();
}

}
