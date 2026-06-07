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
    std::vector<Obstacle> obstacles;  // 障碍物vector
    std::vector<Obstacle> healingObs; // 治疗物vector 

    // 生命值HUD纹理
    sf::Texture heartTexture;
    bool heartTexLoaded = false;
    std::vector<sf::Sprite> heartSprites;

    // 蛋HUD纹理
    sf::Texture eggTexture;
    bool eggTexLoaded = false;
    std::vector<sf::Sprite> eggs;

    // 障碍物生成
    float spawnTimer = 0.f;
    float spawnInterval = 2.0f;       // 初始生成间隔
    float minSpawnInterval = 0.8f;    // 最小间隔
    float difficultyIncrease = 0.05f; // 每次生成后间隔缩短

    sf::Texture obstacleTexture; // 障碍物纹理
    bool obstacleTexLoaded = false;

    // 治疗物生成
    float spawnTimerHeal = 0.f;
    float spawnIntervalHeal = 7.f;    // 初始生成间隔
    float maxSpawnInterval = 32.f;    // 最大间隔
    float difficultyIncreaseHeal = 1.f; // 每次生成一次治疗物品间隔增加

    sf::Texture healingTexture; // 治疗物品
    bool healingObTexLoaded = false;

    // 音乐
    sf::Music bgm;

    // 计分
    int score = 0;
    sf::Font font;
    bool fontLoaded = false;

    // 地面
    sf::Texture groundTexture;
    bool groundTextureLoaded = false;

    // 暂停
    bool paused = false;

    std::mt19937 rng;
    std::uniform_real_distribution<float> obstacleYDist;
    std::uniform_real_distribution<float> healingYDist;

    void checkHealth();
    void spawnObstacle();
    void spawnHeal();
    void checkCollisions();
    void cleanupObstacles();

public:
    explicit GameState(Game& game);
    void handleInput() override;
    void update(sf::Time deltaTime) override;
    void draw() override;
};

#endif // !GAME_STATE_HPP
