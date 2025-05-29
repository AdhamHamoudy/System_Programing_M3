// Email: adhamhamoudy3@gmail.com
#include "Player.hpp"
#include "Game.hpp"

using namespace std;

namespace coup {

Player::Player(Game& game, const string& name)
    : player_name(name), game(game) {
    if (game.players().size() >= 6) {
        throw runtime_error("Cannot add more than 6 players.");
    }
    game.add_player(this);
}

string Player::name() const {
    return player_name;
}

int Player::coins() const {
    return coin_count;
}

string Player::role() const {
    return role_name;
}

void Player::on_sanctioned_by(Player& attacker) {
    (void)attacker;
}

void Player::validate_turn() const {
    if (!is_active) {
        throw runtime_error("Eliminated players cannot play.");
    }
    if (game.turn() != player_name) {
        throw runtime_error("It's not your turn.");
    }
    if (coin_count >= 10) {
        throw runtime_error("You must perform a coup when holding 10 or more coins.");
    }
}

void Player::end_turn() {
    if (used_bribe) {
        used_bribe = false;  // Allow extra move, don't end turn
        return;
    }
    was_arrested = false;
    under_sanction = false;
    game.advance_turn();
}

bool Player::active() const {
    return is_active;
}

void Player::eliminate() {
    is_active = false;
    game.eliminate(this);
}

void Player::add_coins(int amount) {
    coin_count += amount;
}

void Player::remove_coins(int amount) {
    if (coin_count < amount) {
        throw runtime_error("Not enough coins.");
    }
    coin_count -= amount;
}

void Player::gather() {
    validate_turn();
    if (under_sanction) {
        throw runtime_error("Cannot perform Gather/Tax due to active Sanction. Turn is skipped.");
    }
    last_action = "gather";
    add_coins(1);
    end_turn();
}

void Player::tax() {
    validate_turn();
    if (under_sanction) {
        throw runtime_error("Cannot perform Gather/Tax due to active Sanction. Turn is skipped.");
    }
    last_action = "tax";
    add_coins(2);
    end_turn();
}

void Player::bribe() {
    validate_turn();
    if (coin_count < 4) {
        throw runtime_error("Not enough coins to bribe.");
    }
    last_action = "bribe";
    remove_coins(4);
    used_bribe = true;
    // No end_turn() to allow another action this turn
}

void Player::block_arrest() {
    was_arrested = true;
}

void Player::arrest(Player& target) {
    validate_turn();
    if (last_target == target.name()) {
        throw runtime_error("Cannot arrest the same player twice in a row.");
    }
    if (!target.active()) {
        throw runtime_error("Target already eliminated.");
    }
    if (target.was_arrested) {
        throw runtime_error("Target already arrested this round.");
    }

    last_action = "arrest";
    target.was_arrested = true;

    if (target.role() == "Merchant") {
        target.on_arrested_by(*this);
    } else {
        target.remove_coins(1);
        target.on_arrested_by(*this);
    }

    this->add_coins(1);
    last_target = target.name();
    end_turn();
}

void Player::sanction(Player& target) {
    validate_turn();
    if (coin_count < 3) {
        throw runtime_error("Not enough coins for sanction.");
    }
    if (!target.active()) {
        throw runtime_error("Target is already eliminated.");
    }
    last_action = "sanction";
    remove_coins(3);
    target.under_sanction = true;

    target.on_sanctioned_by(*this);
    end_turn();
}

void Player::set_coup_blocked(bool value) {
    coup_blocked = value;
}

void Player::coup(Player& target) {
    if (!is_active) {
        throw runtime_error("Eliminated players cannot play.");
    }
    if (game.turn() != player_name) {
        throw runtime_error("It's not your turn.");
    }
    if (!target.active()) {
        throw runtime_error("Target already eliminated.");
    }
    last_action = "coup";
    game.coup(this, &target);
}

bool Player::is_coup_blocked() const {
    return coup_blocked;
}

} // namespace coup
