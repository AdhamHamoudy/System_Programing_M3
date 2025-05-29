//Email:adhamhamoudy3@gmail.com
#pragma once

#include <string>
#include <vector>
#include <stdexcept>

namespace coup {

class Player;  // forward declaration

class Game {
private:
    std::vector<Player*> active_players;
    size_t current_turn_index = 0;

public:
    Game();
    void add_player(Player* player);
    std::vector<std::string> players() const;
    std::string turn() const;
    std::string winner() const;
    void eliminate(Player* player);
    void advance_turn();
    void coup(Player* attacker, Player* target);

    // friend access to Player
    friend class Player;
};

}
