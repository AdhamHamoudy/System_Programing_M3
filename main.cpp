// Email: adhamhamoudy3@gmail.com
#include "Player.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"
#include "Game.hpp"

#include <iostream>
#include <vector>
#include <exception>

using namespace std;
using namespace coup;

void print_players(const Game& game) {
    cout << "\nCurrent players: ";
    for (const string& name : game.players()) cout << name << " ";
    cout << endl;
}

// Helper to advance the game to the given player's turn
void advance_to_turn(Game& game, const string& name, vector<Player*>& all) {
    while (game.turn() != name) {
        for (auto* p : all) {
            if (game.turn() == p->name()) {
                p->gather();
                break;
            }
        }
    }
}

int main() {
    Game game{};

    // Add players (order matters)
    Governor gov(game, "Anna");
    Spy spy(game, "Ben");
    Baron baron(game, "Cleo");
    General general(game, "Dan");
    Judge judge(game, "Ella");
    Merchant merchant(game, "Fay");

    vector<Player*> all = {&gov, &spy, &baron, &general, &judge, &merchant};

    // Show initial player list and first turn
    print_players(game);
    cout << "First turn: " << game.turn() << endl;

    // Each player gathers once, in the correct order
    for (size_t i = 0; i < all.size(); ++i) {
        cout << "\nTurn: " << game.turn() << endl;
        all[i]->gather();
        cout << all[i]->name() << " coins: " << all[i]->coins() << endl;
    }

    // Try an invalid out-of-turn action (should throw)
    cout << "\nAttempt out-of-turn action:" << endl;
    try {
        gov.gather(); // Not Anna's turn
    } catch (const exception& e) {
        cerr << e.what() << endl;
    }

    // Governor does tax (ensure it's Anna's turn!)
    advance_to_turn(game, "Anna", all);
    cout << "\nTurn: " << game.turn() << endl;
    gov.tax();
    cout << "Anna (Governor) coins after tax: " << gov.coins() << endl;

    // Spy gathers, then uses spy_on ability on Baron
    advance_to_turn(game, "Ben", all);
    cout << "\nTurn: " << game.turn() << endl;
    spy.gather();
    try {
        spy.spy_on(baron);
    } catch (const exception& e) {
        cerr << "Spy failed to spy: " << e.what() << endl;
    }

    // Baron tries to invest with not enough coins (should throw)
    advance_to_turn(game, "Cleo", all);
    cout << "\nTurn: " << game.turn() << endl;
    try {
        baron.invest();
    } catch (const exception& e) {
        cerr << "Baron failed to invest: " << e.what() << endl;
    }

    // Baron taxes, then invests (now has enough coins)
    baron.tax();    // 2 coins
    advance_to_turn(game, "Dan", all); general.gather();
    advance_to_turn(game, "Ella", all); judge.gather();
    advance_to_turn(game, "Fay", all); merchant.gather();

    advance_to_turn(game, "Cleo", all);
    baron.invest(); // should now have 6 coins
    cout << "Baron (Cleo) coins after invest: " << baron.coins() << endl;

    // General gathers, then tries to block coup (invalid, but demo for exception)
    advance_to_turn(game, "Dan", all);
    general.gather();
    try {
        general.block_coup(baron);
    } catch (const exception& e) {
        cerr << "General failed to block coup: " << e.what() << endl;
    }

    // Judge gathers, then tries to cancel bribe (nobody bribed yet)
    advance_to_turn(game, "Ella", all);
    judge.gather();
    try {
        judge.cancel_bribe(spy);
    } catch (const exception& e) {
        cerr << "Judge failed to cancel bribe: " << e.what() << endl;
    }

    // Merchant gathers, shows bonus
    advance_to_turn(game, "Fay", all);
    merchant.gather();
    cout << "Merchant (Fay) coins: " << merchant.coins() << endl;

    // Governor sanctions Merchant (make sure it's Anna's turn!)
    advance_to_turn(game, "Anna", all);
    gov.sanction(merchant);
    cout << "Fay under sanction.\n";

    // Advance to Merchant's turn, try to act; if both actions fail, skip turn to avoid crash
    advance_to_turn(game, "Fay", all);
    bool acted = false;
    try {
        merchant.tax();
        acted = true;
    } catch (const exception& e) {
        cerr << "Merchant under sanction (tax): " << e.what() << endl;
    }
    if (!acted) {
        try {
            merchant.gather();
            acted = true;
        } catch (const exception& e) {
            cerr << "Merchant under sanction (gather): " << e.what() << endl;
            // Skip turn if still sanctioned and cannot act!
            merchant.end_turn();
        }
    }

    // Simulate Baron gaining coins for a coup
    while (baron.coins() < 7) {
        advance_to_turn(game, "Cleo", all);
        baron.gather();
    }
    cout << "Baron coins before coup: " << baron.coins() << endl;

    // Wait for Baron's turn and coup General
    advance_to_turn(game, "Cleo", all);
    try {
        baron.coup(general);
        cout << "Baron performed coup on Dan (General)" << endl;
    } catch (const exception& e) {
        cerr << "Baron failed to coup: " << e.what() << endl;
    }

    print_players(game);

    // Simulate eliminating all but one (Anna), always acting in-turn
    auto in_game = [&]() {
        vector<Player*> res;
        for (auto p : all) {
            for (auto& n : game.players()) {
                if (p->name() == n && p->active()) res.push_back(p);
            }
        }
        return res;
    };

    try {
        // Improved elimination: if nobody can coup, make all gather until someone can.
        while (game.players().size() > 1) {
            Player* attacker = nullptr;
            Player* victim = nullptr;

            // Find an attacker (someone with at least 7 coins)
            for (auto p : in_game()) {
                if (p->coins() >= 7 && p->active()) {
                    attacker = p;
                    break;
                }
            }

            // If nobody can coup, make everyone gather until someone can
            if (!attacker) {
                for (auto p : in_game()) {
                    advance_to_turn(game, p->name(), all);
                    p->gather();
                }
                continue; // Re-evaluate after all have gathered
            }

            // Find a victim (anyone except the attacker)
            for (auto p : in_game()) {
                if (p != attacker && p->active()) {
                    victim = p;
                    break;
                }
            }

            if (!attacker || !victim) break;
            advance_to_turn(game, attacker->name(), all);
            attacker->coup(*victim);
            cout << attacker->name() << " coups " << victim->name() << endl;
        }
    } catch (const exception& e) {
        cerr << "Error during eliminations: " << e.what() << endl;
    }

    // Print winner
    try {
        cout << "\nWinner is: " << game.winner() << endl;
    } catch (const exception& e) {
        cerr << "No winner: " << e.what() << endl;
    }

    return 0;
}
