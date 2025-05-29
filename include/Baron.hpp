// Email: adhamhamoudy3@gmail.com
#pragma once

#include "Player.hpp"

namespace coup {

class Baron : public virtual Player{
public:
    Baron(Game& game, const std::string& name);

    // Special ability: invest 3 to gain 6
    void invest();

    // For identifying in base class
    bool isBaron() const override { return true; }
    void on_sanctioned_by(Player& attacker) override;

};

}
