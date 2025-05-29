// Email: adhamhamoudy3@gmail.com
// Complete GUI for Coup Game (SFML), bugfixes for elimination, coup block, sanction logic

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <set>
#include <iostream>
#include <sstream>
#include <random>
#include <ctime>
#include "Game.hpp"
#include "Governor.hpp"
#include "Spy.hpp"
#include "Baron.hpp"
#include "General.hpp"
#include "Judge.hpp"
#include "Merchant.hpp"

using namespace coup;
using namespace std;

enum class GameState {
    SELECT_PLAYER_COUNT,
    ENTER_PLAYER_NAMES,
    GAME_RUNNING,
    SELECT_TARGET_ACTION,
    GAME_OVER
};

std::set<std::string> spy_revealed_players;
std::string pendingAction;
Player* pendingPlayer = nullptr;

struct ActionButton {
    sf::FloatRect rect;
    std::string label;
};

Player* createRandomPlayer(Game& game, const std::string& name) {
    static std::vector<std::string> roles = {
        "Governor", "Spy", "Baron", "General", "Judge", "Merchant"
    };
    static std::default_random_engine rng(static_cast<unsigned>(time(nullptr)));
    std::uniform_int_distribution<int> dist(0, roles.size() - 1);
    std::string role = roles[dist(rng)];

    if (role == "Governor") return new Governor(game, name);
    if (role == "Spy") return new Spy(game, name);
    if (role == "Baron") return new Baron(game, name);
    if (role == "General") return new General(game, name);
    if (role == "Judge") return new Judge(game, name);
    if (role == "Merchant") return new Merchant(game, name);
    throw std::runtime_error("Invalid role selected.");
}

std::vector<ActionButton> drawGameRunning(sf::RenderWindow& window, sf::Font& font, Game& game, const std::vector<Player*>& players, bool gameOver = false)
{
    std::vector<ActionButton> actionButtons;
    Player* current = nullptr;
    for (Player* p : players) {
        if (p->active() && p->name() == game.turn()) {
            current = p;
            break;
        }
    }

    float yOffset = 20;
    sf::Text title;
    if (!gameOver) {
        title.setString("Coup Game - Turn: " + (current ? current->name() : ""));
        title.setFillColor(sf::Color::Yellow);
    } else {
        title.setString("Game Over!");
        title.setFillColor(sf::Color::Red);
    }
    title.setFont(font);
    title.setCharacterSize(28);
    title.setPosition(20, yOffset);
    window.draw(title);
    yOffset += 40;

    sf::Text playerTitle("Players:", font, 24);
    playerTitle.setPosition(30, yOffset);
    playerTitle.setFillColor(sf::Color::White);
    window.draw(playerTitle);
    yOffset += 35;

    // Show each player; eliminated are gray and say (Eliminated)
    for (Player* p : players) {
        std::string coins = (p == current || spy_revealed_players.count(p->name())) ? std::to_string(p->coins()) : "?";
        std::string role = (p == current) ? p->role() : "?";
        std::string state = (p->active() ? "" : " (Eliminated)");

        sf::Text playerText(p->name() + " - 💰 " + coins + " - Role: " + role + state, font, 20);
        playerText.setPosition(40, yOffset);
        if (!p->active()) {
            playerText.setFillColor(sf::Color(120, 120, 120));  // Gray for eliminated
        } else {
            playerText.setFillColor(p == current ? sf::Color::Cyan : sf::Color::White);
        }
        window.draw(playerText);
        yOffset += 30;
    }

    if (!gameOver && current) {
        std::vector<std::string> buttonLabels = {
            "GATHER", "TAX", "BRIBE", "ARREST", "SANCTION", "COUP"
        };
        if (current->role() == "Spy") buttonLabels.push_back("SPY ON");
        if (current->role() == "Baron") buttonLabels.push_back("INVEST");
        if (current->role() == "Governor") buttonLabels.push_back("UNDO TAX");
        if (current->role() == "Judge") buttonLabels.push_back("CANCEL BRIBE");

        float btnStartX = 650;
        float btnStartY = 120;
        float btnWidth = 180;
        float btnHeight = 50;
        float spacing = 15;

        for (size_t i = 0; i < buttonLabels.size(); ++i) {
            sf::RectangleShape button(sf::Vector2f(btnWidth, btnHeight));
            button.setPosition(btnStartX, btnStartY + i * (btnHeight + spacing));
            button.setFillColor(sf::Color(50, 100, 200));
            window.draw(button);

            sf::Text label(buttonLabels[i], font, 22);
            label.setFillColor(sf::Color::White);
            label.setPosition(btnStartX + 20, btnStartY + i * (btnHeight + spacing) + 10);
            window.draw(label);

            actionButtons.push_back({sf::FloatRect(btnStartX, btnStartY + i * (btnHeight + spacing), btnWidth, btnHeight), buttonLabels[i]});
        }
    }
    return actionButtons;
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Coup Game - Modern GUI");
    sf::Font font;
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf")) {
        cerr << "Failed to load font!" << endl;
        return -1;
    }

    Game game;
    GameState state = GameState::SELECT_PLAYER_COUNT;
    int playerCount = 0;
    vector<string> playerNames;
    vector<Player*> players;
    string currentInput;
    int currentNameIndex = 0;
    std::string logMessage;
    std::string winnerName;

    // For showing "last action" even after error, until next player acts:
    std::string turnPersistentMessage;

    while (window.isOpen()) {
        window.clear(sf::Color(24, 26, 34));
        std::vector<ActionButton> actionButtons;

        if (state == GameState::SELECT_PLAYER_COUNT) {
            sf::Text title("Select number of players (2-6):", font, 36);
            title.setFillColor(sf::Color::White);
            title.setPosition(250, 150);
            window.draw(title);

            sf::RectangleShape inputBox(sf::Vector2f(200, 50));
            inputBox.setPosition(400, 240);
            inputBox.setFillColor(sf::Color(40, 40, 60));
            window.draw(inputBox);

            sf::Text inputText(currentInput, font, 32);
            inputText.setFillColor(sf::Color::Cyan);
            inputText.setPosition(410, 245);
            window.draw(inputText);

            sf::RectangleShape startButton(sf::Vector2f(200, 60));
            startButton.setPosition(400, 420);
            startButton.setFillColor(sf::Color(100, 149, 237));
            window.draw(startButton);

            sf::Text startLabel("Start", font, 30);
            startLabel.setFillColor(sf::Color::White);
            startLabel.setPosition(460, 430);
            window.draw(startLabel);

            if (!logMessage.empty()) {
                sf::Text log(logMessage, font, 20);
                log.setFillColor(sf::Color::Red);
                log.setPosition(320, 320);
                window.draw(log);
            }
        }

        if (state == GameState::ENTER_PLAYER_NAMES) {
            stringstream ss;
            ss << "Enter name for player " << (currentNameIndex + 1) << ":";
            sf::Text prompt(ss.str(), font, 30);
            prompt.setFillColor(sf::Color::White);
            prompt.setPosition(250, 150);
            window.draw(prompt);

            sf::RectangleShape inputBox(sf::Vector2f(300, 50));
            inputBox.setPosition(350, 240);
            inputBox.setFillColor(sf::Color(40, 40, 60));
            window.draw(inputBox);

            sf::Text nameInput(currentInput, font, 30);
            nameInput.setFillColor(sf::Color::Cyan);
            nameInput.setPosition(360, 245);
            window.draw(nameInput);

            sf::RectangleShape nextButton(sf::Vector2f(200, 60));
            nextButton.setPosition(400, 420);
            nextButton.setFillColor(sf::Color(46, 204, 113));
            window.draw(nextButton);

            sf::Text nextLabel("Next", font, 28);
            nextLabel.setFillColor(sf::Color::Black);
            nextLabel.setPosition(465, 430);
            window.draw(nextLabel);

            if (!logMessage.empty()) {
                sf::Text log(logMessage, font, 20);
                log.setFillColor(sf::Color::Red);
                log.setPosition(320, 320);
                window.draw(log);
            }
        }

        if (state == GameState::GAME_RUNNING) {
            actionButtons = drawGameRunning(window, font, game, players);
        }

        if (state == GameState::GAME_OVER) {
            drawGameRunning(window, font, game, players, true);
            sf::Text winLabel("Winner: " + winnerName, font, 40);
            winLabel.setFillColor(sf::Color::Green);
            winLabel.setPosition(350, 350);
            window.draw(winLabel);
            sf::Text restart("Press R to restart", font, 25);
            restart.setFillColor(sf::Color::Cyan);
            restart.setPosition(380, 400);
            window.draw(restart);
            if (!logMessage.empty()) {
                sf::Text log(logMessage, font, 20);
                log.setFillColor(sf::Color::Red);
                log.setPosition(320, 450);
                window.draw(log);
            }
        }

        if (state == GameState::SELECT_TARGET_ACTION) {
            float y = 160;
            sf::Text txt("Select a target for " + pendingAction + ":", font, 30);
            txt.setFillColor(sf::Color::White);
            txt.setPosition(200, 70);
            window.draw(txt);

            for (Player* p : players) {
                if (p == pendingPlayer) continue;
                if (!p->active()) continue;
                sf::RectangleShape btn(sf::Vector2f(300, 50));
                btn.setPosition(350, y);
                btn.setFillColor(sf::Color(39, 174, 96));
                window.draw(btn);

                sf::Text nameTxt(p->name(), font, 24);
                nameTxt.setFillColor(sf::Color::White);
                nameTxt.setPosition(370, y + 10);
                window.draw(nameTxt);

                y += 70;
            }

            if (!logMessage.empty()) {
                sf::Text log(logMessage, font, 20);
                log.setFillColor(sf::Color::Red);
                log.setPosition(320, y + 20);
                window.draw(log);
            }
        }

        // Always draw log at bottom in GAME_RUNNING for errors and actions
        if (state == GameState::GAME_RUNNING && (!logMessage.empty() || !turnPersistentMessage.empty())) {
            sf::Text log;
            log.setFont(font);
            log.setCharacterSize(20);
            log.setPosition(20, 650);
            log.setFillColor(sf::Color::Red);
            if (!logMessage.empty()) {
                log.setString(logMessage);
            } else if (!turnPersistentMessage.empty()) {
                log.setString(turnPersistentMessage);
            }
            window.draw(log);
        }

        window.display();

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (state == GameState::GAME_OVER && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                playerCount = 0;
                playerNames.clear();
                players.clear();
                currentInput.clear();
                currentNameIndex = 0;
                spy_revealed_players.clear();
                pendingAction.clear();
                pendingPlayer = nullptr;
                winnerName.clear();
                logMessage.clear();
                turnPersistentMessage.clear();
                game = Game();
                state = GameState::SELECT_PLAYER_COUNT;
            }

            if (state == GameState::SELECT_PLAYER_COUNT) {
                if (event.type == sf::Event::TextEntered) {
                    if (isdigit(event.text.unicode)) {
                        currentInput += static_cast<char>(event.text.unicode);
                    } else if (event.text.unicode == 8 && !currentInput.empty()) {
                        currentInput.pop_back();
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
                    sf::Vector2f mouse = window.mapPixelToCoords(mousePixel);
                    if (sf::FloatRect(400, 420, 200, 60).contains(mouse)) {
                        if (!currentInput.empty()) {
                            try {
                                int num = std::stoi(currentInput);
                                if (num >= 2 && num <= 6) {
                                    playerCount = num;
                                    state = GameState::ENTER_PLAYER_NAMES;
                                    currentInput = "";
                                    logMessage = "";
                                } else {
                                    logMessage = "Error: Player count must be between 2 and 6!";
                                }
                            } catch (...) {
                                logMessage = "Error: Enter a valid number (2-6)!";
                            }
                        } else {
                            logMessage = "Error: Please enter number of players.";
                        }
                    }
                }
            }

            if (state == GameState::ENTER_PLAYER_NAMES) {
                if (event.type == sf::Event::TextEntered) {
                    if (event.text.unicode == 8 && !currentInput.empty()) {
                        currentInput.pop_back();
                    } else if (event.text.unicode < 128 && isprint(event.text.unicode)) {
                        currentInput += static_cast<char>(event.text.unicode);
                    }
                }
                if (event.type == sf::Event::MouseButtonPressed &&
                    event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
                    sf::Vector2f mouse = window.mapPixelToCoords(mousePixel);
                    if (sf::FloatRect(400, 420, 200, 60).contains(mouse)) {
                        if (!currentInput.empty()) {
                            playerNames.push_back(currentInput);
                            currentInput.clear();
                            currentNameIndex++;
                            if (currentNameIndex == playerCount) {
                                for (const string& name : playerNames) {
                                    players.push_back(createRandomPlayer(game, name));
                                }
                                state = GameState::GAME_RUNNING;
                            }
                        }
                    }
                }
            }

            if (state == GameState::GAME_RUNNING
                && event.type == sf::Event::MouseButtonPressed
                && event.mouseButton.button == sf::Mouse::Left)
            {
                sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f mouse = window.mapPixelToCoords(mousePixel);
                for (const auto& btn : actionButtons) {
                    if (btn.rect.contains(mouse)) {
                        Player* current = nullptr;
                        for (Player* p : players) {
                            if (p->active() && p->name() == game.turn()) {
                                current = p;
                                break;
                            }
                        }
                        if (!current) break;
                        try {
                            if (btn.label == "GATHER") {
                                current->gather();
                                logMessage = current->name() + " gathered 1 coin.";
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;
                            } else if (btn.label == "TAX") {
                                current->tax();
                                logMessage = current->name() + " took tax.";
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;
                            } else if (btn.label == "BRIBE") {
                                current->bribe();
                                logMessage = current->name() + " used bribe (gets an extra action).";
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;
                            } else if (btn.label == "INVEST") {
                                Baron* baron = dynamic_cast<Baron*>(current);
                                if (!baron) throw std::runtime_error("Not a Baron.");
                                baron->invest();
                                logMessage = current->name() + " invested 3 coins to get 6!";
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;
                            } else if (btn.label == "SPY ON") {
                                Spy* spy = dynamic_cast<Spy*>(current);
                                if (!spy) throw std::runtime_error("Not a Spy.");
                                bool found = false;
                                for (Player* p : players) {
                                    if (p->active() && p != current) {
                                        spy_revealed_players.insert(p->name());
                                        found = true;
                                    }
                                }
                                if (found) {
                                    logMessage = current->name() + " spied on all players!";
                                } else {
                                    logMessage = "No valid targets for Spy On.";
                                    spy_revealed_players.clear();
                                }
                                turnPersistentMessage = logMessage;
                            } else if (btn.label == "ARREST" || btn.label == "SANCTION" || btn.label == "COUP"
                                || btn.label == "UNDO TAX" || btn.label == "CANCEL BRIBE") {
                                state = GameState::SELECT_TARGET_ACTION;
                                pendingAction = btn.label;
                                pendingPlayer = current;
                                logMessage = "Select a target for " + btn.label;
                                turnPersistentMessage.clear();
                            }
                        } catch (const std::exception& ex) {
                            logMessage = std::string("Error: ") + ex.what();
                            spy_revealed_players.clear();
                            // End turn if sanctioned on GATHER or TAX
                            std::string err = ex.what();
                            if ((btn.label == "GATHER" || btn.label == "TAX") && err.find("sanction") != std::string::npos) {
                                current->end_turn();
                            }
                        }
                        break;
                    }
                }
                if (logMessage.find("Error:") == std::string::npos) logMessage.clear();
            }

            if (state == GameState::SELECT_TARGET_ACTION &&
                event.type == sf::Event::MouseButtonPressed &&
                event.mouseButton.button == sf::Mouse::Left) {

                float y = 160;
                sf::Vector2i mousePixel(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f mouse = window.mapPixelToCoords(mousePixel);
                bool endEventLoop = false;
                for (Player* p : players) {
                    if (p == pendingPlayer) continue;
                    if (!p->active()) continue;

                    sf::FloatRect area(350, y, 300, 50);
                    if (area.contains(mouse)) {
                        try {
                            if (pendingAction == "ARREST") {
                                pendingPlayer->arrest(*p);
                                logMessage = pendingPlayer->name() + " arrested " + p->name() + ".";
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;
                                state = GameState::GAME_RUNNING;
                            } else if (pendingAction == "SANCTION") {
                                pendingPlayer->sanction(*p);
                                logMessage = pendingPlayer->name() + " sanctioned " + p->name() + ".";
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;
                                state = GameState::GAME_RUNNING;
                            } else if (pendingAction == "COUP") {
                                int coinsBefore = p->coins();
                                bool wasActive = p->active();
                                pendingPlayer->coup(*p);
                                if (p->role() == "General" && coinsBefore >= 5 && p->coins() == coinsBefore - 5 && wasActive && p->active()) {
                                    logMessage = p->name() + " blocked the coup! (Paid 5 coins)";
                                } else if (!p->active()) {
                                    logMessage = pendingPlayer->name() + " performed coup on " + p->name() + " (Eliminated).";
                                } else {
                                    logMessage = pendingPlayer->name() + " performed coup on " + p->name() + ".";
                                }
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;

                                // Winner check/fix
                                if (game.players().size() == 1) {
                                    try { winnerName = game.winner(); }
                                    catch (...) { winnerName = ""; }
                                    if (!winnerName.empty()) {
                                        state = GameState::GAME_OVER;
                                        logMessage = "Game Over! Winner: " + winnerName;
                                        turnPersistentMessage = logMessage;
                                        endEventLoop = true;
                                        break;
                                    }
                                }
                                if (state != GameState::GAME_OVER) {
                                    state = GameState::GAME_RUNNING;
                                }
                            } else if (pendingAction == "SPY ON") {
                                Spy* spy = dynamic_cast<Spy*>(pendingPlayer);
                                if (!spy) throw std::runtime_error("Not a Spy.");
                                spy->spy_on(*p);
                                spy_revealed_players.insert(p->name());
                                logMessage = pendingPlayer->name() + " spied on " + p->name() + ".";
                                turnPersistentMessage = logMessage;
                                state = GameState::GAME_RUNNING;
                            } else if (pendingAction == "UNDO TAX") {
                                Governor* gov = dynamic_cast<Governor*>(pendingPlayer);
                                if (!gov) throw std::runtime_error("Not a Governor.");
                                gov->undo(*p);
                                logMessage = pendingPlayer->name() + " undid tax for " + p->name() + ".";
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;
                                state = GameState::GAME_RUNNING;
                            } else if (pendingAction == "CANCEL BRIBE") {
                                Judge* judge = dynamic_cast<Judge*>(pendingPlayer);
                                if (!judge) throw std::runtime_error("Not a Judge.");
                                judge->cancel_bribe(*p);
                                logMessage = pendingPlayer->name() + " canceled bribe for " + p->name() + ".";
                                spy_revealed_players.clear();
                                turnPersistentMessage = logMessage;
                                state = GameState::GAME_RUNNING;
                            }
                        } catch (const std::exception& ex) {
                            logMessage = std::string("Error: ") + ex.what();
                            spy_revealed_players.clear();
                            turnPersistentMessage.clear();
                            state = GameState::GAME_RUNNING;
                        }
                        break;
                    }
                    y += 70;
                }
                if (endEventLoop) {
                    break;
                }
            }
        }

        // Winner check (fallback, for non-coup win conditions)
        if (state == GameState::GAME_RUNNING && game.players().size() == 1) {
            try { winnerName = game.winner(); }
            catch (...) { winnerName = ""; }
            if (!winnerName.empty()) {
                state = GameState::GAME_OVER;
                logMessage = "Game Over! Winner: " + winnerName;
                turnPersistentMessage = logMessage;
            }
        }
    }

    for (Player* p : players) delete p;
    return 0;
}
