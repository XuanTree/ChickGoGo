#ifndef ENTITY_HPP
#define ENTITY_HPP

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <string>

// ==================== Chick ====================

class Chick {
private:
    std::unique_ptr<sf::Sprite> sprite;
    const sf::Texture* texture = nullptr;

    // 动画
    int frameCount = 14;
    int frameWidth = 0;
    int frameHeight = 0;
    int currentFrame = 0;
    float animationTimer = 0.f;
    static constexpr float animationSpeed = 0.02f;

    // 物理
    sf::Vector2f velocity{0.f, 0.f};
    static constexpr float gravity = 980.f;
    static constexpr float jumpVelocity = -650.f;

    // 边界（地面 Y，在 GameState 中设置）
    float groundY = 500.f;
    float leftBound = 50.f;
    float rightBound = 750.f;

    // 状态
    int health = 3;
    int maxHealth = 5;
    bool isOnGround = true;
    bool isJumpKeyHeld = false;
    sf::Clock jumpHoldClock;
    float jumpHoldDuration = 0.f;
    static constexpr float maxJumpHoldTime = 3.2f;
    static constexpr float jumpBoostForce = 600.f;
    bool invincible = false;
    float invincibleTimer = 0.f;
    static constexpr float invincibleDuration = 1.5f;

    // 音效
    sf::SoundBuffer jumpBuffer;
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer healBuffer;
    sf::SoundBuffer scoreBuffer;
    std::unique_ptr<sf::Sound> hitSound;
    std::unique_ptr<sf::Sound> jumpSound;
    std::unique_ptr<sf::Sound> healSound;
    std::unique_ptr<sf::Sound> scoreSound;
    bool soundsLoaded = false;

public:
    Chick() = default;

    void init(const sf::Texture* tex, int frameCount,
              float startX, float startY, float groundY,
              float scale = 1.0f);
    bool loadSounds(const std::string& jumpPath, const std::string& hitPath, 
        const std::string& scorePath, const std::string& healPath);

    void handleInput();
    void update(float dt);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    void takeDamage();
    int getHealth() const { return health; }
    void setHealth(int newHealth);
    int getMaxHealth() const;
    bool isDead() const { return health <= 0; }
    bool isInvincible() const { return invincible; }

    void reset(float x, float y);
    sf::Vector2f getPosition() const { return sprite ? sprite->getPosition() : sf::Vector2f{0.f, 0.f}; }

    // Get Chick Sound
    sf::Sound& getHitSound() { return *hitSound; }
    sf::Sound& getJumpSound() { return *jumpSound; }
    sf::Sound& getScoreSound() { return *scoreSound; }
    sf::Sound& getHealSound() { return *healSound; }
};

// ==================== Obstacle ====================

class Obstacle {
private:
    std::unique_ptr<sf::Sprite> sprite;
    const sf::Texture* texture = nullptr;

    float speed = 200.f;      // 向左移动速度 (px/s)
    float screenWidth = 800.f;
    bool scored = false;      // 玩家是否已经越过这个障碍物（计分用）
    bool consumed = false;    // 治疗物是否已被吃掉

public:
    Obstacle() = default;

    void init(const sf::Texture* tex, float x, float y, float speed, float screenW);
    void update(float dt);
    void draw(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    bool isOffScreen() const;
    bool hasScored() const { return scored; }
    void markScored() { scored = true; }
    bool isConsumed() const { return consumed; }
    void markConsumed() { consumed = true; }
    float getRightEdge() const;
};

#endif // ENTITY_HPP
