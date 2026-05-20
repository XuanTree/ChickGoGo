#include "Game.hpp"
#include "States/MenuState.hpp"
#include "States/GameState.hpp"
#include <iostream>

void Game::run(Game& game) {
    game.pushState(std::make_unique<MenuState>(game));
    sf::Clock clock;
    sf::Image icon; //BUG 游戏结束的时候图标会消失
    if (!icon.loadFromFile("assets\\sprites\\icon.png")) {
        std::cerr << "icon load failed!" << std::endl;
    } else {
        window.setIcon(icon);
    }
    while (window.isOpen()) {
        sf::Time deltaTime = clock.restart();
        if (!states.empty()) {
            states.top()->handleInput();
            states.top()->update(deltaTime);
            window.clear();
            states.top()->draw();
            window.display();
            // 在一帧结束后再执行替换，保证不会在状态自身方法中销毁该对象
            if (pendingReplace) {
                if (!states.empty()) {
                    states.pop();
                }
                if (pendingState) {
                    states.push(std::move(pendingState));
                }
                pendingReplace = false;
            }
        }
            
    }
}
