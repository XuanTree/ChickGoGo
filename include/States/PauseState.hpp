#ifndef PAUSE_STATE_HPP
#define PAUSE_STATE_HPP

#include "State.hpp"

class Game;
using namespace GameEngine;

// ==================== PauseState ====================

class PauseState : public State {
private:
    Game& game;
    sf::Font font;
    bool fontLoaded = false;

public:
    explicit PauseState(Game& game);
    void handleInput() override;
    void update(sf::Time deltaTime) override;
    void draw() override;
};

#endif // PAUSE_STATE_HPP