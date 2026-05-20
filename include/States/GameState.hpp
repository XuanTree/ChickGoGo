#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP

#pragma once
#include "State.hpp"
#include "Entity.hpp"
#include <vector>
#include <random>

class Game;

using namespace GameEngine;

// ==================== GameState ====================

class GameState : public State {
private:
    Game& game;

    Chick chick;
    std::vector<Obstacle> obstacles;

    // 障碍物生成
    float spawnTimer = 0.f;
    float spawnInterval = 2.0f;       // 初始生成间隔
    float minSpawnInterval = 0.8f;    // 最小间隔
    float difficultyIncrease = 0.05f; // 每次生成后间隔缩短

    // 障碍物纹理（用简单的彩色矩形代替）
    // 如果assets里有障碍物纹理，可以改用纹理
    sf::Texture obstacleTexture;
    bool obstacleTexLoaded = false;

    // 音乐
    sf::Music bgm;

    // 计分
    int score = 0;
    sf::Font font;
    bool fontLoaded = false;

    // 地面
    sf::RectangleShape ground;

    // 暂停
    bool paused = false;

    std::mt19937 rng;
    std::uniform_real_distribution<float> obstacleYDist;

    void spawnObstacle();
    void checkCollisions();
    void cleanupObstacles();

public:
    explicit GameState(Game& game);
    void handleInput() override;
    void update(sf::Time deltaTime) override;
    void draw() override;
};

#endif // !GAME_STATE_HPP
