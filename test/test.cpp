// Email: adhamhamoudy3@gmail.com
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../include/doctest.h"

#include "../include/Game.hpp"
#include "../include/Governor.hpp"
#include "../include/Spy.hpp"
#include "../include/Baron.hpp"
#include "../include/General.hpp"
#include "../include/Judge.hpp"
#include "../include/Merchant.hpp"

using namespace coup;

TEST_CASE("Governor tax gives 3 coins") {
    Game g;
    Governor gov(g, "Alice");
    CHECK(gov.coins() == 0);

    gov.gather();  // +1 coin
    CHECK(gov.coins() == 1);

    g.advance_turn();  // to next player (even if none)
    gov.tax();  // +3 coins
    CHECK(gov.coins() == 4);
}

TEST_CASE("Spy sees coins and blocks arrest") {
    Game g;
    Governor gov(g, "Bob");
    Spy spy(g, "Eve");

    gov.gather();  // +1 coin
    g.advance_turn();  // to Spy

    // Spy spies on Bob
    spy.spy_on(gov);
    CHECK_NOTHROW(spy.spy_on(gov));

    // Bob can't arrest after being spied on
    CHECK_THROWS(gov.arrest(spy));  // because Bob was blocked
}

TEST_CASE("Baron investment gives profit") {
    Game g;
    Baron baron(g, "Ben");

    baron.add_coins(3);  // Manually add coins
    CHECK(baron.coins() == 3);

    baron.invest();  // Use ability
    CHECK(baron.coins() == 6);  // 3 paid, 6 gained
}

TEST_CASE("Baron gets 1 coin when sanctioned") {
    Game g;
    Baron baron(g, "Ben");
    Spy spy(g, "Eve");

    spy.add_coins(3);
    CHECK(spy.coins() == 3);

    g.advance_turn();  // Spy's turn
    spy.sanction(baron);

    CHECK(spy.coins() == 0);    // Spy paid 3
    CHECK(baron.coins() == 1);  // Baron got compensated 1
}

TEST_CASE("General blocks coup successfully") {
    Game g;
    Governor attacker(g, "Attacker");
    General defender(g, "General");

    attacker.add_coins(7);  // Enough to coup
    defender.add_coins(5);  // Enough to block

    // Make sure it's attacker's turn
    while (g.turn() != attacker.name()) {
        g.advance_turn();
    }

    CHECK_NOTHROW(attacker.coup(defender));  // Game logic should trigger block

    CHECK(defender.active());       // General should survive
    CHECK(attacker.coins() == 0);   // Attacker still pays
}

TEST_CASE("General gets coin back when arrested") {
    Game g;
    General general(g, "Gen");
    Governor gov(g, "Gov");

    gov.add_coins(1);
    general.add_coins(1);

    // Ensure it's Gov's turn
    while (g.turn() != gov.name()) {
        g.advance_turn();
    }

    CHECK_NOTHROW(gov.arrest(general));
    CHECK(general.coins() == 1);  // Got coin back after arrest
}

TEST_CASE("Judge cancels bribe and attacker pays extra on sanction") {
    Game g;
    Judge judge(g, "Judge");
    Governor gov(g, "Gov");

    gov.add_coins(4);  // Enough to bribe
    judge.add_coins(0);

    // Make sure it's Gov's turn to perform bribe
    while (g.turn() != gov.name()) g.advance_turn();

    // Gov uses bribe
    CHECK_NOTHROW(gov.bribe());
    CHECK(gov.coins() == 0);

    // Judge cancels the bribe → Gov loses coins (already paid)
    CHECK_NOTHROW(judge.cancel_bribe(gov));
    CHECK(gov.coins() == 0);  // Bribe already removed 4

    // Now Judge gets sanctioned and attacker pays +1 extra
    gov.add_coins(4);  // Enough to perform sanction
    while (g.turn() != gov.name()) g.advance_turn();

    CHECK_NOTHROW(gov.sanction(judge));
    CHECK(gov.coins() == 0);  // 3 for sanction + 1 extra because Judge
}
TEST_CASE("Merchant gains 1 coin at start of turn and pays 2 to bank when arrested") {
    Game g;
    Merchant merchant(g, "Merchant");
    General general(g, "General");

    merchant.add_coins(3);  // Will trigger bonus at start
    CHECK(merchant.coins() == 3);

    // Simulate start of turn logic
    CHECK_NOTHROW(merchant.start_turn_bonus());
    CHECK(merchant.coins() == 4);  // +1 bonus

    // General arrests Merchant — should lose 2 coins to bank
    general.add_coins(1);
    while (g.turn() != general.name()) g.advance_turn();

    CHECK_NOTHROW(general.arrest(merchant));
    CHECK(merchant.coins() == 2);  // Lost 2 to bank
    CHECK(general.coins() == 2);   // Gained 1 from arrest
}

TEST_CASE("Player with 10 coins must perform coup") {
    Game g;
    Governor gov(g, "Gov");
    Spy target(g, "Target");

    gov.add_coins(10);
    CHECK_THROWS(gov.gather());  // Not allowed
    CHECK_THROWS(gov.tax());     // Not allowed
    CHECK_THROWS(gov.sanction(target));  // Not allowed
}

TEST_CASE("Coup with insufficient coins throws") {
    Game g;
    General attacker(g, "Attacker");
    Merchant target(g, "Target");

    attacker.add_coins(6);  // Not enough
    CHECK_THROWS(attacker.coup(target));
}

TEST_CASE("Arresting same player twice in a row is illegal") {
    Game g;
    Spy spy(g, "Spy");
    Governor gov(g, "Gov");

    gov.add_coins(2);
    spy.add_coins(1);

    // First arrest
    while (g.turn() != gov.name()) g.advance_turn();
    CHECK_NOTHROW(gov.arrest(spy));

    // Advance and come back to gov
    g.advance_turn();  // To spy
    g.advance_turn();  // Back to gov

    CHECK_THROWS(gov.arrest(spy));  // Second arrest in a row
}

TEST_CASE("Eliminated player cannot perform actions") {
    Game g;
    General player(g, "Player");
    Judge enemy(g, "Enemy");

    player.eliminate();

    CHECK_THROWS(player.gather());
    CHECK_THROWS(player.tax());
    CHECK_THROWS(player.sanction(enemy));
    CHECK_THROWS(player.coup(enemy));
}
