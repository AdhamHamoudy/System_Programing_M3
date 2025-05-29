// Email: adhamhamoudy3@gmail.com
#pragma once

#include "Player.hpp"

namespace coup {

class Judge : public Player {
public:
    Judge(Game& game, const std::string& name);

    // Cancel bribe
    void cancel_bribe(Player& target);

    // Sanction behavior: attacker pays +1
    void on_sanctioned_by(Player& attacker) override;
};

}
