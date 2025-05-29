// Email: adhamhamoudy3@gmail.com
#pragma once

#include "Player.hpp"

namespace coup {

class Governor : public virtual Player {
public:
    Governor(Game& game, const std::string& name);

    // Override the tax function to give 3 coins instead of 2
    void tax() override;

    // Governor can undo another player's tax
    void undo(Player& other);
};

}
