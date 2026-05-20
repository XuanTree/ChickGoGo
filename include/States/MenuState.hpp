#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "State.hpp"
class Game;

using namespace GameEngine;


// ==================== MenuState ====================

class MenuState : public State {
private:
    Game& game;
    sf::Font font;
    bool fontLoaded = false;

public:
    explicit MenuState(Game& game);
    void handleInput() override;
    void update(sf::Time deltaTime) override;
    void draw() override;
};

#endif // MENU_STATE_HPP
