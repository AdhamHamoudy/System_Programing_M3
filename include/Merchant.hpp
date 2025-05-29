// Email: adhamhamoudy3@gmail.com
#pragma once

#include "Player.hpp"

namespace coup {

class Merchant : public virtual Player {
public:
    Merchant(Game& game, const std::string& name);

    // Apply automatic +1 at start of turn if coins >= 3
    void start_turn_bonus();

    // Override arrest behavior
    void on_arrested_by(Player& by) override;
    void validate_turn() const override;

};

}
