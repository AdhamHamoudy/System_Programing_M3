// Email: adhamhamoudy3@gmail.com
#include "Spy.hpp"
#include <iostream>

using namespace std;

namespace coup {

Spy::Spy(Game& game, const string& name)
    : Player(game, name) {
    role_name = "Spy";
}

void Spy::spy_on(Player& target) {
    if (!is_active) {
        throw runtime_error("Eliminated players cannot spy.");
    }
    if (!target.active()) {
        throw runtime_error("Target is not active.");
    }

    // See target's coins
    std::cerr << "[Spy] " << target.name() << " has " << target.coins() << " coins." << endl;

    // Prevent target from arresting in their next turn
    target.block_arrest();
}

}
