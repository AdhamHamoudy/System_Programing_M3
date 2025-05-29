// Email: adhamhamoudy3@gmail.com
#include "General.hpp"

using namespace std;

namespace coup {

General::General(Game& game, const string& name)
    : Player(game, name) {
    role_name = "General";
}

void General::block_coup(Player& target) {
    if (coins() < 5) {
        throw runtime_error("Not enough coins to block a coup.");
    }
    remove_coins(5);
    target.set_coup_blocked(true);
}

void General::on_arrested_by(Player& by) {
    (void)by;        //again, silence unused warning
    // When arrested, General gets the stolen coin back
    add_coins(1);
}


}
