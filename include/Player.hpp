// Email: adhamhamoudy3@gmail.com
#pragma once

#include <string>
#include <stdexcept>

namespace coup {

class Game;

class Player {
protected:
    std::string player_name;
    Game& game;
    int coin_count = 0;
    std::string role_name;
    bool is_active = true;
    bool was_arrested = false;
    bool under_sanction = false;
    std::string last_target = "";
    bool coup_blocked = false;  // Used for General's block
    bool used_bribe = false;    // Allows a second action in same turn
    std::string last_action = "";  // Track last action for Governor undo

public:
    // Constructor / Destructor
    Player(Game& game, const std::string& name);
    virtual ~Player() = default;

    // Rule of Three - block copy
    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    // Basic info
    std::string name() const;
    int coins() const;
    virtual std::string role() const;

    // Actions - can be overridden by subclasses
    virtual void gather();
    virtual void tax();
    virtual void bribe();
    virtual void arrest(Player& target);
    virtual void sanction(Player& target);
    virtual void coup(Player& target);

    // Role flags and hooks
    virtual bool isBaron() const { return false; }
    virtual void on_sanctioned_by(Player& attacker);
    virtual void on_arrested_by(Player& by) {
        (void)by; // silence unused warning
    }

    // Turn handling
    virtual void validate_turn() const;
    virtual void end_turn();
    virtual void undo(Player& other) {
        (void)other; // prevent unused parameter warning
        throw std::runtime_error(this->role() + " cannot undo.");
    }

    // State queries and mutation
    bool is_coup_blocked() const;
    bool active() const;
    void eliminate();
    void add_coins(int amount);
    void remove_coins(int amount);
    void block_arrest();
    void set_coup_blocked(bool value);

    // Helpers for role logic
    bool has_used_bribe() const { return used_bribe; }
    void set_used_bribe(bool val) { used_bribe = val; }

    std::string get_last_action() const { return last_action; }
    void set_last_action(const std::string& action) { last_action = action; }
    void clear_last_action() { last_action = ""; }
};

} // namespace coup
