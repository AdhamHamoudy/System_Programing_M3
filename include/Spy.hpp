// Email: adhamhamoudy3@gmail.com
#pragma once

#include "Player.hpp"

namespace coup {

class Spy : public Player {
public:
    Spy(Game& game, const std::string& name);

    // Spy action: View target's coins and block arrest
    void spy_on(Player& target);
};

}
