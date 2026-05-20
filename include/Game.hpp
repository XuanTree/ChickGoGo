#ifndef GAME_HPP
#define GAME_HPP

#pragma once
#include <memory>
#include <stack>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "States/State.hpp"
#include "ResourceManager.hpp"

using namespace GameEngine;

class Game {
private:
    sf::RenderWindow window;
    std::stack<std::unique_ptr<State>> states;

    // 延迟的状态替换（在当前帧结束后执行，避免在状态方法内销毁自身）
    std::unique_ptr<State> pendingState;
    bool pendingReplace = false;
public:
    TextureManager chickTexture;
    SpriteManager chickSprite;
    AudioManager chickJumpSound;
    AudioManager chickHitSound;

    Game() {
        window.create(sf::VideoMode({800, 600}), "ChickGOGOGO!!!");
        window.setVerticalSyncEnabled(true);
    }
    void pushState(std::unique_ptr<State> state) {
        states.push(std::move(state));
    }
    void popState() {
        if (!states.empty()) {
            states.pop();
        }
    }
    // 请求在当前帧结束后替换顶层状态（安全，避免在状态成员函数内销毁自身）
    void changeState(std::unique_ptr<State> state) {
        pendingState = std::move(state);
        pendingReplace = true;
    }
    void run(Game& game);
    sf::RenderWindow& getWindow() { return window; }
};

#endif // !GAME_HPP
