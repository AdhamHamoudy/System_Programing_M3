// Email: adhamhamoudy3@gmail.com
#pragma once

#include "Player.hpp"

namespace coup {

class General : public Player {
public:
    General(Game& game, const std::string& name);

    // Special ability: blocks a coup against a player (including self)
    void block_coup(Player& target);

    // Special reaction: gets coin back when arrested
    void on_arrested_by(Player& by) override;
    
};

}
